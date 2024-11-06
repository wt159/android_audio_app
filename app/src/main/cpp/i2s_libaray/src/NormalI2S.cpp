#include "internal/NormalI2S.h"
#include "internal/I2SError.h"
#include "internal/TinyalsaWrapper.h"
#include <cstdio>
#include <sstream>
#include <string.h>
#include <vector>

NormalI2S::NormalI2S()
{
    // 初始化时检查tinyalsa库是否可用
    if (!TinyalsaWrapper::Instance().Initialize()) {
        printf("Error: Failed to initialize tinyalsa library\n");
        throw std::runtime_error("Failed to initialize tinyalsa library");
    }
}

int NormalI2S::isnumber(const char* str)
{
    char* end;

    if (str == NULL || strlen(str) == 0)
        return 0;

    strtol(str, &end, 10);
    return strlen(end) == 0;
}

static std::vector<int> String2Array(const char* str)
{
    std::vector<int> result;
    int value;
    std::istringstream iss(str);

    // 从字符串中读取整数，并添加到向量中
    while (iss >> value) {
        result.push_back(value);
    }

    return result;
}

int NormalI2S::ConfigureMixer()
{
    auto& alsa = TinyalsaWrapper::Instance();
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    struct mixer* mixer = alsa.mixer_open(kCardNum);
    if (!mixer) {
        printf("Error: Failed to open mixer\n");
        return I2S_ERR_MIXER;
    }

    // 配置mixer controls
    const struct {
        const char* name;
        const char* value;
    } mixer_controls[] = {
        { "MultiMedia1 Mixer TERT_TDM_TX_0", "1" }
    };

    for (const auto& ctrl : mixer_controls) {
        struct mixer_ctl* ctl = alsa.mixer_get_ctl_by_name(mixer, ctrl.name);
        type = alsa.mixer_ctl_get_type(ctl);
        num_ctl_values = alsa.mixer_ctl_get_num_values(ctl);
        const char* value = NULL;
        std::vector<int> arr_int;
        printf("%s: control:%s %s type:%d num:%d\n", __func__, ctrl.name, ctrl.value, type, num_ctl_values);
#if 1
        if (isnumber(ctrl.value)) {
            printf("%s: ctrl value is number\n");
            if (num_ctl_values == 1) {
                /* Set all values the same */
                int value = atoi(ctrl.value);

                for (unsigned int i = 0; i < num_ctl_values; i++) {
                    if (alsa.mixer_ctl_set_value(ctl, i, value)) {
                        fprintf(stderr, "Error: invalid value\n");
                        alsa.mixer_close(mixer);
                        return I2S_ERR_MIXER;
                    }
                }
            } else {
                arr_int = String2Array(ctrl.value);
                for (unsigned int i = 0; i < num_ctl_values; i++) {
                    if (i > arr_int.size() - 1)
                        break;
                    if (alsa.mixer_ctl_set_value(ctl, i, arr_int[i])) {
                        fprintf(stderr, "Error: invalid value for index %d\n", i);
                        alsa.mixer_close(mixer);
                        return I2S_ERR_MIXER;
                    }
                }
            }
        } else {
            printf("%s: ctrl value not is number\n", __func__);
            if (type == MIXER_CTL_TYPE_ENUM) {
                if (num_ctl_values != 1) {
                    fprintf(stderr, "Enclose strings in quotes and try again\n");
                    alsa.mixer_close(mixer);
                    return I2S_ERR_MIXER;
                }
                if (alsa.mixer_ctl_set_enum_by_string(ctl, ctrl.value)) {
                    fprintf(stderr, "Error: invalid enum value\n");
                    alsa.mixer_close(mixer);
                    return I2S_ERR_MIXER;
                }
            } else {
                fprintf(stderr, "Error: only enum types can be set with strings\n");
                alsa.mixer_close(mixer);
                return I2S_ERR_MIXER;
            }
        }
#endif
    }

    alsa.mixer_close(mixer);
    return I2S_SUCCESS;
}

int NormalI2S::ConfigurePCM()
{
    struct pcm_config config = {
        .channels = kChannels,
        .rate = kSamplingRate,
        .period_size = kPeriodSize,
        .period_count = kPeriodCount,
        .format = PCM_FORMAT_S16_LE,
        .start_threshold = 0,
        .stop_threshold = 0,
        .silence_threshold = 0,
        .silence_size = 0,
        .avail_min = 0,
    };
    auto& alsa = TinyalsaWrapper::Instance();
    pcm_ = alsa.pcm_open(kCardNum, kDeviceNum, PCM_IN, &config);
    if (!pcm_ || !alsa.pcm_is_ready(pcm_)) {
        if (pcm_) {
            alsa.pcm_close(pcm_);
            pcm_ = nullptr;
        }
        printf("Error: Failed to open PCM\n");
        return I2S_ERR_HARDWARE;
    }

    return I2S_SUCCESS;
}

int NormalI2S::Open()
{
    if (is_opened_) {
        printf("Error: I2S is already opened\n");
        return I2S_ERR_BUSY;
    }

    int ret = ConfigureMixer();
    if (ret != I2S_SUCCESS) {
        return ret;
    }

    ret = ConfigurePCM();
    if (ret != I2S_SUCCESS) {
        return ret;
    }

    is_opened_ = true;
    return I2S_SUCCESS;
}

int NormalI2S::Read(void* buffer, int size)
{
    if (!is_opened_) {
        printf("Error: I2S is not ready\n");
        return I2S_ERR_NOT_READY;
    }

    if (!buffer || size <= 0) {
        printf("Error: Invalid buffer or size\n");
        return I2S_ERR_INVALID_SIZE;
    }

    if (size % (kChannels * 2) != 0) { // 确保size是帧大小的整数倍
        printf("Error: Invalid size, must be a multiple of frame size\n");
        return I2S_ERR_INVALID_SIZE;
    }
    auto& alsa = TinyalsaWrapper::Instance();
    return alsa.pcm_read(pcm_, buffer, size);
}

int NormalI2S::Close()
{
    if (!is_opened_) {
        printf("Error: I2S is not ready to close\n");
        return I2S_ERR_NOT_READY;
    }
    auto& alsa = TinyalsaWrapper::Instance();
    if (pcm_) {
        alsa.pcm_close(pcm_);
        pcm_ = nullptr;
    }
    alsa.Cleanup();

    is_opened_ = false;
    return I2S_SUCCESS;
}