#include "internal/HighSpeedI2S.h"
#include "HighSpeedI2S.h"
#include "hsi2s_common.h"
#include "internal/I2SError.h"
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define BYTES_PER_WORD 4
#define READ_LENGTH_MB 4
#define READ_LENGTH_WORDS (READ_LENGTH_MB * 1024 * 1024) / BYTES_PER_WORD
// #define kReadLimit 2048000 * 8 * 60 /* 60s of ramp data */
#define SRC_DIGITAL_PLL 0x500
#define BILLION 1000000000L
#define DAB_TUNER_COUNT 3
#define PG_SIZE 4096
#define SHM_SIZE (PG_SIZE * 3)
#define SHM_WRDMA_BASE 0
#define SHM_WRDMA_CURRENT 1

void* HighSpeedI2S::poll_read_normal(void* arg)
{
    unsigned long long r_limit = 0;
    long temp;
    int ret;
    double thread_start;
    double thread_stop;
    double delta;
    struct timespec pread_start;
    struct timespec pread_stop;
    struct pollfd pfd;
    char* mmap_ptr;
    char* mmap_read;
    char* mmap_end;
    HighSpeedI2S* params;

    params = static_cast<HighSpeedI2S*>(arg);
    if (params) {
        pfd = params->pfd;
        mmap_ptr = static_cast<char*>(params->mmap_ptr);
        mmap_read = static_cast<char*>(params->mmap_read);
        mmap_end = static_cast<char*>(params->mmap_end);
        printf("mmap end - read: %ld\n", mmap_end - mmap_read);
        printf("mmap end - write: %ld\n", mmap_end - mmap_ptr);
    } else {
        printf("Thread parameters are null\n");
        return NULL;
    }

    printf("Performing poll wait for normal read...\n");

    clock_gettime(CLOCK_REALTIME, &pread_start);
    thread_start = (pread_start.tv_sec * BILLION) + pread_start.tv_nsec;
    printf("[POLL] Starttime %lf\n", thread_start);

    while (params->is_opened_) {
        clock_gettime(CLOCK_REALTIME, &pread_start);
        ret = poll(&pfd, 1, -1);
        clock_gettime(CLOCK_REALTIME, &pread_stop);
        delta = ((pread_stop.tv_sec - pread_start.tv_sec) * BILLION) + (pread_stop.tv_nsec - pread_start.tv_nsec);
        // printf("[POLL] Data ready in %lf nsec\n", delta);
        if (ret < 0) {
            printf("Poll failed\n");
        } else if (pfd.revents & EPOLLIN) {
            if (mmap_read + kMmapLen > mmap_end) {
                temp = mmap_end - mmap_read;
                params->write_op(mmap_read, temp);
                temp = kMmapLen - temp;
                params->write_op(mmap_ptr, temp);
                mmap_read = mmap_ptr + temp;
            } else {
                params->write_op(mmap_read, kMmapLen);
                mmap_read += kMmapLen;
            }
            r_limit += kMmapLen;
            if (mmap_read >= mmap_end)
                mmap_read = mmap_ptr;
        }
        // if (r_limit > kMmapLen*5) break;
    }

    clock_gettime(CLOCK_REALTIME, &pread_stop);
    thread_stop = (pread_stop.tv_sec * BILLION) + pread_stop.tv_nsec;
    delta = (thread_stop - thread_start) / BILLION;
    printf("[POLL] Endtime %lf\n", thread_stop);
    printf("[POLL] Total thread execution time %lfs\n", delta);

    return NULL;
}

int HighSpeedI2S::Open()
{
    int ret = 0;
    fd_master = open(kHsDevName, O_RDWR);
    if (fd_master < 0) {
        printf("Cannot open core device file\n");
        ret = -1;
    }
    fd_core = open(kCoreDevName, O_RDWR);
    if (fd_core < 0) {
        printf("Cannot open core device file\n");
        ret = -1;
    }
    printf("Setting normal mode \n");
    ret = ioctl(fd_master, LPAIF_RESET);
    if (ret < 0) {
        printf("Failed to reset the hsi2s device\n");
    }
    ret = ioctl(fd_master, LPAIF_NORMAL_MODE);
    if (ret < 0) {
        printf("Failed to configure normal operation on the hsi2s device\n");
    }

    printf("Periodic length set to %ld bytes\n", kMmapLen);

    /* Map the device write DMA buffer */
    this->pfd.fd = fd_master;
    this->pfd.events = EPOLLIN | EPOLLRDNORM;
    printf("Mapping userspace memory with kernel memory for device\n");
    this->mmap_ptr = mmap(NULL, kReadLengthBytes * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd_master, 0);
    if (this->mmap_ptr == MAP_FAILED) {
        printf("mmap failed for device\n");
        ret = -1;
        goto error;
    } else {
        this->mmap_read = this->mmap_ptr;
        this->mmap_end = static_cast<char*>(this->mmap_ptr) + (kReadLengthBytes * 2);
    }
    printf("%s: mmap bytes:%ld\n", __func__, kReadLengthBytes*2);
    printf("%s: mmap len  :%ld\n", __func__, kMmapLen);
    printf("%s: read limit:%ld\n", __func__, kReadLimit);
    printf("Using mmap mode...\n");

    /* Create thread to read the received data */
    is_opened_ = true;
    thread_ = new std::thread(poll_read_normal, this);
    if (thread_ == nullptr) {
        printf("Failed to create thread\n");
        ret = -1;
        goto error;
    }

    return ret;

error:
    if (fd_core) {
        close(fd_core);
        fd_core = -1;
    }
    if (fd_master) {
        close(fd_master);
        fd_master = -1;
    }
    is_opened_ = false;
    return ret;
}

int HighSpeedI2S::Read(void* buffer, int size)
{
    bool ret = rb_.get((char*)buffer, size);
    if (ret)
        return size;
    return I2S_ERR_INVALID_SIZE;
}

int HighSpeedI2S::Close()
{
    is_opened_ = false;
    if (thread_) {
        thread_->join();
        delete thread_;
        thread_ = nullptr;
    }
    if (fd_core) {
        close(fd_core);
        fd_core = -1;
    }
    if (fd_master) {
        close(fd_master);
        fd_master = -1;
    }
    return I2S_SUCCESS;
}
int HighSpeedI2S::write_op(const void* ptr, size_t size)
{
    // printf("%s: size = %zu\n", __func__, size);
    bool ret = rb_.put((const char*)ptr, size);
    if (!ret) {
        printf("%s: write ring buffer failed, %zu\n", __func__, size);
    }
    return 0;
}
