#ifndef NORMAL_I2S_H
#define NORMAL_I2S_H

#include "II2S.h"
#include <memory>
#include <tinyalsa/asoundlib.h>

class NormalI2S : public II2S {
public:
    NormalI2S();
    ~NormalI2S() {};

    int Open() override;
    int Read(void* buffer, int size) override;
    int Close() override;

private:
    int ConfigureMixer();
    int ConfigurePCM();
    int isnumber(const char* str);

    static constexpr int kCardNum = 0;
    static constexpr int kDeviceNum = 0;
    static constexpr int kSamplingRate = 48000;
    static constexpr int kChannels = 2;
    static constexpr int kPeriodSize = 1024;
    static constexpr int kPeriodCount = 4;

    struct pcm* pcm_ = nullptr;
};

#endif // NORMAL_I2S_H