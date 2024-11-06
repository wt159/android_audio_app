#ifndef COMMAND_EXECUTOR_H
#define COMMAND_EXECUTOR_H

#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>

class CommandExecutor {
public:
    CommandExecutor()
    {
        // 在构造函数中执行命令
        executeCommands();
    }

private:
    void executeCommands()
    {
        std::vector<std::string> commands = {
            "cd /data/local/tmp && ./si479x_radio_my DAB", // 替换为实际命令
            "tinymix 'TERT_TDM_TX_0 SampleRate' 'KHZ_16'",
            "tinymix 'TERT_TDM_TX_0 Channels' 'Two'",
            "tinymix 'TERT_TDM SlotNumber' 'Two'",
            "tinymix 'TERT_TDM_TX_0 SlotMapping' 0 4 65535 0 0 0 0",
        };

        std::vector<std::string> commands2 = {
            "insmod /vendor/lib/modules/hsi2s.ko lpaif_mode=1 enable_qmi=1 bit_clock_hz=32768000",
            "hsi2s_test --op_mode=10 --dev=/dev/hs0_i2s --pcm",
            "hsi2s_test --op_mode=11 --dev=/dev/hs0_i2s --lane_config=1",
            "hsi2s_test --op_mode=6 --dev=/dev/hs0_i2s --clk_src=1 --divide_by=7",
            "hsi2s_test --op_mode=5 --dev=/dev/hs0_i2s --slave",
            "hsi2s_test --op_mode=7 --dev=/dev/hs0_i2s --bit_clock_hz=32768000 --data_buffer_ms=10 --bit_depth=16 --spkr_ch=2 --mic_ch=2",
            "hsi2s_test --op_mode=8 --dev=/dev/hs0_i2s --bit_clock_hz=32768000 --data_buffer_ms=10 --pcm_rate=5 --pcm_sync_ext --pcm_sync_short --rpcm_width=1 --tpcm_width=1",
            "hsi2s_test --op_mode=9 --dev=/dev/hs0_i2s --tdm_sync_delay=2 --tdm_tpcm_width=16 --tdm_rpcm_width=16 --tdm_rate=32",
        };

        /*判断 /dev/hs0_i2s 设备节点是否存在，如果没有，就执行commands2 */
        if (access("/dev/hs0_i2s", F_OK) == -1) {
            for (const auto& cmd : commands) {
                system(cmd.c_str()); // 执行命令
            }

            for (const auto& cmd : commands2) {
                system(cmd.c_str()); // 执行命令
            }
        }
    }
};

#endif // COMMAND_EXECUTOR_H