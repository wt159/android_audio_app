#include "internal/I2SFactory.h"
#include "internal/HighSpeedI2S.h"
#include "internal/NormalI2S.h"
#include <memory>

std::unique_ptr<II2S> I2SFactory::CreateI2S(I2SMode mode)
{
    switch (mode) {
    case I2SMode::NORMAL:
        return std::unique_ptr<II2S>(new NormalI2S());
    case I2SMode::HIGH_SPEED:
        return std::unique_ptr<II2S>(new HighSpeedI2S());
    default:
        return nullptr;
    }
}