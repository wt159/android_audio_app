#ifndef I2S_API_H
#define I2S_API_H

#ifdef __cplusplus
extern "C" {
#endif

/* I2S错误码定义 */
#define I2S_SUCCESS 0 /* 操作成功 */
#define I2S_ERR_BASE (-1000) /* I2S错误码基准值 */
#define I2S_ERR_INVALID_MODE (I2S_ERR_BASE - 1) /* 无效的工作模式 */
#define I2S_ERR_NO_RESOURCE (I2S_ERR_BASE - 2) /* 资源不足，无法创建新实例 */
#define I2S_ERR_HARDWARE (I2S_ERR_BASE - 3) /* 硬件错误 */
#define I2S_ERR_INVALID_FD (I2S_ERR_BASE - 4) /* 无效的设备句柄 */
#define I2S_ERR_NULL_PTR (I2S_ERR_BASE - 5) /* 空指针错误 */
#define I2S_ERR_INVALID_SIZE (I2S_ERR_BASE - 6) /* 无效的数据大小 */
#define I2S_ERR_TIMEOUT (I2S_ERR_BASE - 7) /* 操作超时 */
#define I2S_ERR_BUSY (I2S_ERR_BASE - 8) /* 设备忙 */
#define I2S_ERR_NOT_READY (I2S_ERR_BASE - 9) /* 设备未就绪 */
#define I2S_ERR_IO (I2S_ERR_BASE - 10) /* I/O错误 */
#define I2S_ERR_MIXER (I2S_ERR_BASE - 11) /* 混音器操作错误 */

/* I2S工作模式 */
typedef enum {
    I2S_MODE_NORMAL = 0, /* FM/AM 48000hz 2ch 16bit */
    I2S_MODE_HIGH_SPEED = 1 /* DAB  32.768Mhz 2048fs 16bit */
} i2s_mode_t;

/**
 * @brief 打开I2S设备
 *
 * @param mode I2S工作模式
 *             - I2S_MODE_NORMAL: 普通模式，适用于FM/AM，48KHz采样率
 *             - I2S_MODE_HIGH_SPEED: 高速模式，适用于DAB，32.768MHz采样率
 *
 * @return int 返回设备句柄或错误码
 *         - >= 0: 成功，返回有效的设备句柄
 *         - < 0: 失败，具体错误码见上述定义
 */
int i2s_open(i2s_mode_t mode);

/**
 * @brief 从I2S设备读取数据
 *
 * @param handle 设备句柄，由i2s_open返回的有效句柄
 * @param buffer 数据缓冲区指针
 * @param size   要读取的字节数
 *
 * @return int 返回实际读取的字节数或错误码
 *         - > 0: 成功读取的字节数
 *         - <= 0: 失败，具体错误码见上述定义
 */
int i2s_read(int handle, void* buffer, int size);

/**
 * @brief 关闭I2S设备
 *
 * @param handle 设备句柄，由i2s_open返回的有效句柄
 *
 * @return int 返回操作结果
 *         - I2S_SUCCESS: 成功
 *         - < 0: 失败，具体错误码见上述定义
 */
int i2s_close(int handle);

#ifdef __cplusplus
}
#endif

#endif // I2S_API_H