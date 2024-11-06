#ifndef II2S_H
#define II2S_H

/**
 * @brief I2S设备接口基类
 */
class II2S {
public:
    virtual ~II2S() = default;

    /**
     * @brief 打开并初始化I2S设备
     * @return int 错误码
     */
    virtual int Open() = 0;

    /**
     * @brief 读取I2S数据
     * @param buffer 数据缓冲区
     * @param size 要读取的字节数
     * @return int 实际读取的字节数或错误码
     */
    virtual int Read(void* buffer, int size) = 0;

    /**
     * @brief 关闭I2S设备
     * @return int 错误码
     */
    virtual int Close() = 0;

protected:
    bool is_opened_ = false;
};

#endif // II2S_H