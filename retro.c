#include <stdio.h>
#include "neopixel.h"

#define GPIO_PIN                18
#define LED_COUNT               7

uint32_t colors[] =
{
    0x00200000,  // red
    0x00201000,  // orange
    0x00202000,  // yellow
    0x00002000,  // green
    0x00002020,  // lightblue
    0x00000020,  // blue
    0x00100010,  // purple
    0x00200010,  // pink
    0x00202020,  // white
};

/*

Strip layout: 

 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17
41                                                   18
40                                                   19
39                                                   20
 38 37 36 35 34 33 32 31 30 29 28 27 26 25 24 23 22 21

*/

unsigned int top[18] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 };
unsigned int bottom[18] = { 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38 };
unsigned int left[3] = { 18, 19, 20 };
unsigned int right[3] = { 39, 40, 41 };

int main(void)
{
    printf("Initializing...\n");
    if (!neo_init(GPIO_PIN, LED_COUNT))
    {
        return 1;
    }

    int ix = 0;
    while (!neo_loop_stop())
    {
        strip_fill(colors[ix]);
        strip_render();
        neo_sleep(0.25);

        strip_clear();
        strip_render();
        neo_sleep(0.10);

        ix = (ix + 1) % ARRAY_SIZE(colors);
    }

    printf("Cleaning up\n");
    neo_deinit();

    return 0;
}
