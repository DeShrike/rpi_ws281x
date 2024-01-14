#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

#include <stdint.h>
#include <stdbool.h>
#include "ws2811.h"

#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

bool neo_init(int pin, int ledcount);
void neo_deinit();
bool neo_loop_stop();

void strip_render();
void strip_clear();
void strip_set_rgb(unsigned int index, unsigned char R, unsigned char G, unsigned char B);
void strip_fill_rgb(unsigned char R, unsigned char G, unsigned char B);
void strip_set(unsigned int index, uint32_t color);
void strip_fill(uint32_t color);
void strip_shift_up();
void strip_shift_down();

void neo_sleep(float second);

#endif
