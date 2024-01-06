#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neopixel.h"

#define GPIO_PIN                18
#define LED_COUNT               7

int dotspos[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
ws2811_led_t dotcolors[] =
{
    0x00200000,  // red
    0x00201000,  // orange
    0x00202000,  // yellow
    0x00002000,  // green
    0x00002020,  // lightblue
    0x00000020,  // blue
    0x00100010,  // purple
    0x00200010,  // pink
};

int main(int argc, char *argv[])
{
    if (!neo_init(GPIO_PIN, LED_COUNT))
    {
        return 1;
    }
    
    while (!neo_loop_stop())
    {
        // Fill to RED
        strip_fill_rgb(30, 0, 0);
        strip_render();
        neo_sleep(0.20);

        // Fill to GREEN
        strip_fill_rgb(0, 30, 0);
        strip_render();
        neo_sleep(0.20);

        // Fill to BLUE
        strip_fill_rgb(0, 0, 30);
        strip_render();
        neo_sleep(0.20);
    }

    neo_deinit();

    return 0;
}
