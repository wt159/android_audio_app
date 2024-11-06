#ifndef I2S_FACTORY_H
#define I2S_FACTORY_H

#include "II2S.h"
#include <memory>

enum class I2SMode {
    NORMAL, // FM/AM 48000hz 2ch 16bit
    HIGH_SPEED // DAB  32.768Mhz 2048fs 16bit
};

class I2SFactory {
public:
    /**
     * @brief 创建I2S实例
     * @param mode I2S工作模式
     * @return std::unique_ptr<II2S> I2S实例指针，失败返回nullptr
     */
    static std::unique_ptr<II2S> CreateI2S(I2SMode mode);
};

#endif // I2S_FACTORY_H