#include "internal/TinyalsaWrapper.h"
#include <stdio.h>

bool TinyalsaWrapper::Initialize()
{
    if (handle_) {
        return true;
    }

    // 尝试加载动态库
    handle_ = dlopen("libtinyalsa.so", RTLD_LAZY);
    if (!handle_) {
        handle_ = dlopen("libtinyalsa.so.2", RTLD_LAZY);
    }
    if (!handle_) {
        printf("Failed to load tinyalsa library: %s\n", dlerror());
        return false;
    }

    // 加载所有需要的函数
    *(void**)(&mixer_open) = dlsym(handle_, "mixer_open");
    *(void**)(&mixer_close) = dlsym(handle_, "mixer_close");
    *(void**)(&mixer_get_ctl_by_name) = dlsym(handle_, "mixer_get_ctl_by_name");
    *(void**)(&mixer_ctl_get_type) = dlsym(handle_, "mixer_ctl_get_type");
    *(void**)(&mixer_ctl_get_num_values) = dlsym(handle_, "mixer_ctl_get_num_values");
    *(void**)(&mixer_ctl_set_enum_by_string) = dlsym(handle_, "mixer_ctl_set_enum_by_string");
    *(void**)(&mixer_ctl_set_value) = dlsym(handle_, "mixer_ctl_set_value");
    *(void**)(&mixer_ctl_set_array) = dlsym(handle_, "mixer_ctl_set_array");
    *(void**)(&pcm_open) = dlsym(handle_, "pcm_open");
    *(void**)(&pcm_close) = dlsym(handle_, "pcm_close");
    *(void**)(&pcm_read) = dlsym(handle_, "pcm_read");
    *(void**)(&pcm_is_ready) = dlsym(handle_, "pcm_is_ready");

    // 检查是否所有函数都加载成功
    if (!mixer_open || !mixer_close || !mixer_get_ctl_by_name || !mixer_ctl_set_enum_by_string || !mixer_ctl_set_array || !pcm_open || !pcm_close || !pcm_read || !pcm_is_ready) {
        printf("Failed to load tinyalsa functions: %s\n", dlerror());
        Cleanup();
        return false;
    }

    return true;
}

void TinyalsaWrapper::Cleanup()
{
    if (handle_) {
        dlclose(handle_);
        handle_ = nullptr;
    }
}