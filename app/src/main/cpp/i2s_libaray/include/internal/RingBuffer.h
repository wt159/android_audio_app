#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring> // for memcpy

class RingBuffer {
public:
    explicit RingBuffer(size_t size) 
        : buffer(new char[roundup_pow_of_two(size)]), 
          max_size(roundup_pow_of_two(size)), 
          head(0), 
          tail(0), 
          full(false) {}

    ~RingBuffer() {
        delete[] buffer;
    }

    bool isFull() const {
        return full;
    }

    bool isEmpty() const {
        return (!full && (head == tail));
    }

    size_t capacity() const {
        return max_size;
    }

    size_t size() const {
        if (full) {
            return max_size;
        }
        if (head >= tail) {
            return head - tail;
        }
        return max_size + head - tail;
    }

    // 写入数据块（线程安全）
    bool put(const char* data, size_t length) {
        std::unique_lock<std::mutex> lock(mtx);

        if (length > freeSpace()) {
            return false; // 缓冲区空间不足
        }

        size_t first_part = std::min(length, max_size - (head & (max_size - 1)));
        std::memcpy(buffer + (head & (max_size - 1)), data, first_part);
        head = (head + first_part) & (max_size - 1);

        if (length > first_part) {
            // 如果写入的数据跨越缓冲区的尾部，从头部继续写入
            std::memcpy(buffer, data + first_part, length - first_part);
            head = (head + (length - first_part)) & (max_size - 1);
        }

        full = (head == tail);
        cv.notify_all(); // 通知读取线程
        return true;
    }

    // 读取数据块（线程安全）
    bool get(char* data, size_t length) {
        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this, length] { return size() >= length; });

        size_t first_part = std::min(length, max_size - (tail & (max_size - 1)));
        std::memcpy(data, buffer + (tail & (max_size - 1)), first_part);
        tail = (tail + first_part) & (max_size - 1);

        if (length > first_part) {
            // 如果读取的数据跨越缓冲区的尾部，从头部继续读取
            std::memcpy(data + first_part, buffer, length - first_part);
            tail = (tail + (length - first_part)) & (max_size - 1);
        }

        full = false;
        cv.notify_all(); // 通知写入线程
        return true;
    }

    size_t freeSpace() const {
        return max_size - size();
    }

private:
    // 将任意大小向上取整为 2 的幂次
    static size_t roundup_pow_of_two(size_t x) {
        if (x <= 1) return 1;
        x--;                    // 先减1，防止原来就是2的幂
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        if (sizeof(size_t) > 4) // 如果是64位机器
            x |= x >> 32;
        return x + 1;
    }

    char* buffer;
    const size_t max_size;
    size_t head;
    size_t tail;
    bool full;
    std::mutex mtx;
    std::condition_variable cv;
};

// // 写入线程函数
// void writer(RingBuffer& rb, const char* data, size_t length) {
//     while (true) {
//         if (rb.put(data, length)) {
//             std::cout << "Written data: " << std::string(data, length) << std::endl;
//         } else {
//             std::cout << "Buffer full, unable to write data." << std::endl;
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(5));
//     }
// }

// // 读取线程函数
// void reader(RingBuffer& rb, size_t length) {
//     char* data = new char[length];
//     while (true) {
//         if (rb.get(data, length)) {
//             std::cout << "Read data: " << std::string(data, length) << std::endl;
//         } else {
//             std::cout << "Buffer empty, unable to read data." << std::endl;
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(5));
//     }
//     delete[] data;
// }

// int main() {
//     RingBuffer rb(1024); // 创建一个初始大小为 1024 字节的环形缓冲区
//     const char* sampleData = "ABCDE";

//     std::thread writer_thread(writer, std::ref(rb), sampleData, strlen(sampleData));
//     std::thread reader_thread(reader, std::ref(rb), strlen(sampleData));

//     writer_thread.join();
//     reader_thread.join();

//     return 0;
// }
