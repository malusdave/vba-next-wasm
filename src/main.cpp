#include <stdio.h>
#include <emscripten.h>
#include <stdlib.h>
#include "libretro.h"

struct retro_game_info gameInfo;

void wasmLogCb(enum retro_log_level level, const char *fmt, ...)
{
}

void wasmVideoCb(const void *data, unsigned width,
                 unsigned height, size_t pitch)
{
    static uint8_t rgbaBuffer[240 * 160 * 4];
    uint8_t *ptr = rgbaBuffer;
    uint16_t *srcPtr = (uint16_t *)data;
    unsigned padding = (pitch / 2) - width;

    for (unsigned y = 0; y < 160; y++)
    {
        for (unsigned x = 0; x < 240; x++)
        {
            uint16_t c = *srcPtr;
            uint8_t r = ((c & 0xF800) >> 11) << 3;
            uint8_t g = ((c & 0x7E0) >> 5) << 2;
            uint8_t b = ((c & 0x1F)) << 3;
            ptr[0] = r;
            ptr[1] = g;
            ptr[2] = b;
            ptr[3] = 0xFF;
            ptr += 4;
            srcPtr++;
        }
        srcPtr += padding;
    }

    //printf("%p %d %d %d\n", data, width, height, pitch);
    //printf("video cb\n");
    EM_ASM_({
        drawFrame($0);
    },
           rgbaBuffer);
}

bool wasmEnvironCb(unsigned cmd, void *data)
{
    return false;
}

void wasmPollCb(void)
{
}

int16_t wasmInputCb(unsigned port, unsigned device,
                    unsigned index, unsigned id)
{
    return 0;
}

size_t wasmAudioBatchCb(const int16_t *data, size_t frames)
{
    EM_ASM_({
        writeAudio($0, $1);
    },
           data, frames);
    return 0;
}

extern "C"
{

    int loadRom(void *romBuffer, uint32_t romSize)
    {
        retro_set_audio_sample_batch(wasmAudioBatchCb);
        retro_set_video_refresh(wasmVideoCb);
        retro_set_environment(wasmEnvironCb);
        retro_set_input_poll(wasmPollCb);
        retro_set_input_state(wasmInputCb);
        retro_init();
        gameInfo.path = "game.gba";
        gameInfo.data = romBuffer;
        gameInfo.size = romSize;
        int ret = retro_load_game(&gameInfo) ? 0 : -1;
        return ret;
    }

    int runFrame()
    {
        retro_run();
        return 0;
    }

    int main()
    {

        EM_ASM(
            wasmReady(););
        return 0;
    }
}
