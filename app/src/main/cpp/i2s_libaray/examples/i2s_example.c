#include <i2s_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <stdint.h>

#define BUFFER_SIZE 4096  // 缓冲区大小

// Normal I2S参数
#define NORMAL_SAMPLE_RATE 48000
#define NORMAL_CHANNELS    2
#define NORMAL_BITS        16

// HS-I2S参数
#define HS_SAMPLE_RATE    2048000
#define HS_FRAME_SIZE     4

static volatile int running = 1;

// 信号处理函数，用于优雅退出
static void signal_handler(int signo) {
    (void)signo;
    running = 0;
}

// 错误码转字符串
static const char* error_to_string(int error) {
    switch (error) {
        case I2S_SUCCESS:          return "Success";
        case I2S_ERR_INVALID_MODE: return "Invalid mode";
        case I2S_ERR_NO_RESOURCE:  return "No resource";
        case I2S_ERR_HARDWARE:     return "Hardware error";
        case I2S_ERR_INVALID_FD:   return "Invalid file descriptor";
        case I2S_ERR_NULL_PTR:     return "Null pointer";
        case I2S_ERR_INVALID_SIZE: return "Invalid size";
        case I2S_ERR_TIMEOUT:      return "Timeout";
        case I2S_ERR_BUSY:         return "Device busy";
        case I2S_ERR_NOT_READY:    return "Device not ready";
        case I2S_ERR_IO:           return "I/O error";
        case I2S_ERR_MIXER:        return "Mixer error";
        default:                   return "Unknown error";
    }
}

// 保存WAV文件头
static void write_wav_header(FILE* fp, uint32_t data_size) {
    // RIFF chunk
    fwrite("RIFF", 1, 4, fp);
    uint32_t chunk_size = data_size + 36;
    fwrite(&chunk_size, 4, 1, fp);
    fwrite("WAVE", 1, 4, fp);

    // fmt chunk
    fwrite("fmt ", 1, 4, fp);
    uint32_t fmt_chunk_size = 16;
    fwrite(&fmt_chunk_size, 4, 1, fp);
    uint16_t audio_format = 1; // PCM
    fwrite(&audio_format, 2, 1, fp);
    uint16_t num_channels = NORMAL_CHANNELS;
    fwrite(&num_channels, 2, 1, fp);
    uint32_t sample_rate = NORMAL_SAMPLE_RATE;
    fwrite(&sample_rate, 4, 1, fp);
    uint32_t byte_rate = NORMAL_SAMPLE_RATE * NORMAL_CHANNELS * (NORMAL_BITS/8);
    fwrite(&byte_rate, 4, 1, fp);
    uint16_t block_align = NORMAL_CHANNELS * (NORMAL_BITS/8);
    fwrite(&block_align, 2, 1, fp);
    uint16_t bits_per_sample = NORMAL_BITS;
    fwrite(&bits_per_sample, 2, 1, fp);

    // data chunk
    fwrite("data", 1, 4, fp);
    fwrite(&data_size, 4, 1, fp);
}

static void print_usage(const char* program) {
    printf("Usage: %s [options] <output_file>\n", program);
    printf("Options:\n");
    printf("  -m, --mode <mode>    I2S mode (normal or hs, default: normal)\n");
    printf("  -h, --help           Show this help message\n");
    printf("\n");
    printf("Examples:\n");
    printf("  %s -m normal output.wav    # Record audio in normal I2S mode\n", program);
    printf("  %s -m hs output.dab        # Record data in HS-I2S mode\n", program);
}

int main(int argc, char *argv[]) {
    int ret;
    int handle;
    char buffer[BUFFER_SIZE];
    FILE* output_file = NULL;
    uint32_t total_bytes = 0;
    i2s_mode_t mode = I2S_MODE_NORMAL;
    int is_hs_mode = 0;

    // 命令行选项
    static struct option long_options[] = {
        {"mode", required_argument, 0, 'm'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // 解析命令行参数
    int opt;
    while ((opt = getopt_long(argc, argv, "m:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm':
                if (strcmp(optarg, "hs") == 0) {
                    mode = I2S_MODE_HIGH_SPEED;
                    is_hs_mode = 1;
                } else if (strcmp(optarg, "normal") == 0) {
                    mode = I2S_MODE_NORMAL;
                } else {
                    printf("Invalid mode: %s\n", optarg);
                    print_usage(argv[0]);
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // 检查是否提供了输出文件名
    if (optind >= argc) {
        printf("Error: No output file specified\n");
        print_usage(argv[0]);
        return 1;
    }
    printf("%s: argc:%d, optind:%d\n", __func__, argc, optind);

    // 检查文件扩展名
    const char* filename = argv[optind];
    const char* ext = strrchr(filename, '.');
    if (!ext) {
        printf("Error: Output file must have an extension (.wav or .dab)\n");
        return 1;
    }
    if (is_hs_mode && strcmp(ext, ".dab") != 0) {
        printf("Error: HS-I2S mode requires .dab file extension\n");
        return 1;
    }
    if (!is_hs_mode && strcmp(ext, ".wav") != 0) {
        printf("Error: Normal I2S mode requires .wav file extension\n");
        return 1;
    }

    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 打开输出文件
    output_file = fopen(filename, "wb");
    if (!output_file) {
        printf("Failed to open output file: %s\n", filename);
        return 1;
    }

    // 如果是普通I2S模式，写入WAV文件头
    if (!is_hs_mode) {
        write_wav_header(output_file, 0);
    }

    // 打开I2S设备
    printf("Opening I2S device in %s mode...\n", is_hs_mode ? "HS-I2S" : "normal I2S");
    handle = i2s_open(mode);
    if (handle < 0) {
        printf("Failed to open I2S device: %s\n", error_to_string(handle));
        fclose(output_file);
        return 1;
    }

    printf("Starting data capture...\n");
    printf("Press Ctrl+C to stop recording\n");

    // 主循环：读取数据并写入文件
    while (running) {
        ret = i2s_read(handle, buffer, BUFFER_SIZE);
        if (ret < 0) {
            printf("Failed to read data: %s\n", error_to_string(ret));
            break;
        } else if (ret == 0) {
            printf("No data available\n");
            usleep(10000);  // 等待10ms
            continue;
        }

        // 写入数据到文件
        size_t written = fwrite(buffer, 1, ret, output_file);
        if (written != (size_t)ret) {
            printf("Failed to write data to file\n");
            break;
        }
        total_bytes += ret;

        // 显示进度
        float times = 0.0f;
        if (is_hs_mode) {
            times = (float)total_bytes * 1000.0f / (HS_SAMPLE_RATE * HS_FRAME_SIZE);
            printf("\rRecorded %u bytes (%.2f ms)", total_bytes, times);
        } else {
            times = (float)total_bytes * 1000.0f / (NORMAL_SAMPLE_RATE * NORMAL_CHANNELS * (NORMAL_BITS/8));
            printf("\rRecorded %u bytes (%.2f ms)", total_bytes, times);
        }
        fflush(stdout);
        if (times >= 120000.0f) {
            printf("default 60s , break;\n");
            break;
        }
    }

    printf("\nClosing I2S device...\n");

    // 关闭I2S设备
    ret = i2s_close(handle);
    if (ret != I2S_SUCCESS) {
        printf("Failed to close I2S device: %s\n", error_to_string(ret));
    }

    // 如果是普通I2S模式，更新WAV文件头
    if (!is_hs_mode && fseek(output_file, 0, SEEK_SET) == 0) {
        write_wav_header(output_file, total_bytes);
    }

    // 关闭文件
    fclose(output_file);

    printf("Recording completed. Saved to %s\n", filename);
    if (is_hs_mode) {
        printf("Total recorded: %u bytes (%.2f ms)\n", 
               total_bytes,
               (float)total_bytes * 1000.0f / (HS_SAMPLE_RATE * HS_FRAME_SIZE));
    } else {
        printf("Total recorded: %u bytes (%.2f seconds)\n", 
               total_bytes,
               (float)total_bytes / (NORMAL_SAMPLE_RATE * NORMAL_CHANNELS * (NORMAL_BITS/8)));
    }

    return 0;
} 