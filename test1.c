#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "neopixel.h"

#define GPIO_PIN                18
#define LED_COUNT               30

int main(int argc, char *argv[])
{
    printf("Initializing\n");
    printf(" Pin: %d\n", GPIO_PIN);
    printf("Leds: %d\n", LED_COUNT);

    if (!neo_init(GPIO_PIN, LED_COUNT))
    {
        return 1;
    }

    strip_set(0, 0x00200000);  // red
    strip_set(1, 0x00201000);  // orange
    strip_set(2, 0x00202000);  // yellow
    strip_set(3, 0x00002000);  // green
    strip_set(4, 0x00002020);  // lightblue
    strip_set(5, 0x00000020);  // blue
    strip_set(6, 0x00100010);  // purple
    strip_set(7, 0x00200010);  // pink

    time_t t;
    srand((unsigned) time(&t));
    int direction = 1;

    printf("Starting loop\n");
    while (!neo_loop_stop())
    {
        strip_render();
        neo_sleep(0.05);
        if (direction == 1)
        {
            strip_shift_down();
        }
        else
        {
            strip_shift_up();
        }

        if ((rand() % 1000) < 50)
        {
            direction *= -1;
        }
    }

    printf("\nCleanup\n");
    neo_deinit();

    return 0;
}
