#ifndef HIGH_SPEED_I2S_H
#define HIGH_SPEED_I2S_H

#include "II2S.h"
#include "RingBuffer.h"
#include <poll.h>
#include <string>
#include <thread>

class HighSpeedI2S : public II2S {
public:
    HighSpeedI2S() = default;
    ~HighSpeedI2S() override = default;

    int Open() override;
    int Read(void* buffer, int size) override;
    int Close() override;

protected:
    static void* poll_read_normal(void* arg);
    int write_op(const void* ptr, size_t size);

private:
    static constexpr int kBitClk = 32768000; // 32.768MHz
    static constexpr int kBufferMs = 10;
    static constexpr long kReadLengthBytes = 2 * 1024 * 1024;
    static constexpr long kReadLimit = 2048000 * 8 * 1 /* 1s of ramp data */;
    static constexpr long kMmapLen = (((unsigned long long)kBufferMs * kBitClk) / 8000);
    // 其他HSI2S特有的配置参数
    static constexpr const char* kCoreDevName = "/dev/hsi2s_reginfo";
    static constexpr const char* kHsDevName = "/dev/hs0_i2s";
    int fd_core = -1;
    int fd_master = -1;
    bool is_opened_ = false;
    std::thread* thread_ = nullptr;
    struct pollfd pfd;
    void* mmap_ptr = nullptr;
    void* mmap_read = nullptr;
    void* mmap_end = nullptr;
    RingBuffer rb_{kMmapLen*50};
};

#endif // HIGH_SPEED_I2S_H