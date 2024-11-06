#include "i2s_api.h"
#include "CommandExecutor.h"
#include "internal/I2SFactory.h"
#include <map>
#include <mutex>

namespace {
std::map<int, std::unique_ptr<II2S>> g_i2s_instances;
std::mutex g_mutex;
int g_next_handle = 1;

int GetNextHandle()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_next_handle++;
}

II2S* GetInstance(int handle)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_i2s_instances.find(handle);
    return (it != g_i2s_instances.end()) ? it->second.get() : nullptr;
}

CommandExecutor commandExecutor;
}

extern "C" {

int i2s_open(i2s_mode_t mode)
{
    I2SMode cpp_mode;
    switch (mode) {
    case I2S_MODE_NORMAL:
        cpp_mode = I2SMode::NORMAL;
        break;
    case I2S_MODE_HIGH_SPEED:
        cpp_mode = I2SMode::HIGH_SPEED;
        break;
    default:
        return I2S_ERR_INVALID_MODE;
    }

    auto i2s = I2SFactory::CreateI2S(cpp_mode);
    if (!i2s) {
        return I2S_ERR_NO_RESOURCE;
    }

    int ret = i2s->Open();
    if (ret != I2S_SUCCESS) {
        return ret;
    }

    int handle = GetNextHandle();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_i2s_instances[handle] = std::move(i2s);
    }

    return handle;
}

int i2s_read(int handle, void* buffer, int size)
{
    if (!buffer) {
        return I2S_ERR_NULL_PTR;
    }

    auto instance = GetInstance(handle);
    if (!instance) {
        return I2S_ERR_INVALID_FD;
    }

    return instance->Read(buffer, size);
}

int i2s_close(int handle)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_i2s_instances.find(handle);
    if (it == g_i2s_instances.end()) {
        return I2S_ERR_INVALID_FD;
    }

    int ret = it->second->Close();
    if (ret == I2S_SUCCESS) {
        g_i2s_instances.erase(it);
    }
    return ret;
}

} // extern "C"