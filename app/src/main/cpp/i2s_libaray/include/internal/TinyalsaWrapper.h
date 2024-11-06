#ifndef TINYALSA_WRAPPER_H
#define TINYALSA_WRAPPER_H

#include <dlfcn.h>
#include <stdint.h>
#include <tinyalsa/asoundlib.h>

struct mixer;
struct mixer_ctl;
struct pcm;
struct pcm_config;

class TinyalsaWrapper {
public:
    static TinyalsaWrapper& Instance()
    {
        static TinyalsaWrapper instance;
        return instance;
    }

    bool Initialize();
    void Cleanup();
    bool IsLoaded() const { return handle_ != nullptr; }

    // tinyalsa 函数指针
    struct mixer* (*mixer_open)(unsigned int card);
    void (*mixer_close)(struct mixer* mixer);
    struct mixer_ctl* (*mixer_get_ctl_by_name)(struct mixer* mixer, const char* name);
    unsigned int (*mixer_ctl_get_num_values)(struct mixer_ctl* ctl);
    enum mixer_ctl_type (*mixer_ctl_get_type)(struct mixer_ctl* ctl);
    int (*mixer_ctl_set_enum_by_string)(struct mixer_ctl* ctl, const char* string);
    int (*mixer_ctl_set_value)(struct mixer_ctl* ctl, unsigned int id, int value);
    int (*mixer_ctl_set_array)(struct mixer_ctl* ctl, const void* array, size_t count);

    struct pcm* (*pcm_open)(unsigned int card, unsigned int device, unsigned int flags, struct pcm_config* config);
    void (*pcm_close)(struct pcm* pcm);
    int (*pcm_read)(struct pcm* pcm, void* data, unsigned int count);
    int (*pcm_is_ready)(struct pcm* pcm);

private:
    TinyalsaWrapper()
        : handle_(nullptr)
    {
    }
    ~TinyalsaWrapper() { Cleanup(); }
    TinyalsaWrapper(const TinyalsaWrapper&) = delete;
    TinyalsaWrapper& operator=(const TinyalsaWrapper&) = delete;

    void* handle_;
};

#endif // TINYALSA_WRAPPER_H