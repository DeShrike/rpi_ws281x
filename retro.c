#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "neopixel.h"

#define GPIO_PIN                18
#define LED_COUNT               30
#define PATTERN_DURATION        10

typedef void pattern_func(void);

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

#define DURATION (current - starttime)
time_t starttime, current;

void start(void)
{
    time(&starttime);
}

void tick(void)
{
    time(&current);
}

#if 0

/*

Strip layout:

 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
39                                                 17
38                                                 18
37                                                 19
 36 35 34 33 32 31 30 29 28 27 26 25 24 23 22 21 20

*/

unsigned int top[18] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
unsigned int bottom[18] = { 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20 };
unsigned int right[3] = { 17, 18, 19 };
unsigned int left[3] = { 37, 38, 39 };

#else

/*

Strip layout:

 00 01 02 03 04 05 06 07 08 09 10 11
29                                 12
28                                 13
27                                 14
 26 25 24 23 22 21 20 19 18 17 16 15

*/

unsigned int top[12] =    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11 };
unsigned int bottom[12] = { 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15 };
unsigned int right[3] =  { 12, 13, 14 };
unsigned int left[3] = { 27, 28, 29 };

#endif

//#define SLEEP sleep_ms
#define SLEEP neo_sleep
#define HORI (sizeof(top) / sizeof(int))
#define VERT (sizeof(left) / sizeof(int))
#define WIDTH (HORI + 2)
#define HEIHT (VERT + 2)

void fill_left(uint32_t color)
{
    for (int i = 0; i < VERT; i++)
    {
        strip_set(left[i], color);
    }
}

void fill_right(uint32_t color)
{
    for (int i = 0; i < VERT; i++)
    {
        strip_set(right[i], color);
    }
}

void fill_top(uint32_t color)
{
    for (int i = 0; i < HORI; i++)
    {
        strip_set(top[i], color);
    }
}

void fill_bottom(uint32_t color)
{
    for (int i = 0; i < HORI; i++)
    {
        strip_set(bottom[i], color);
    }
}

void sleep_ms(int milliseconds)
{
    // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
    {
        sleep(milliseconds / 1000);
    }

    usleep((milliseconds % 1000) * 1000);
#endif
}

void pattern1(void)
{
    start();
    int ix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_fill(colors[ix]);
        strip_render();
        SLEEP(0.25);

        strip_clear();
        strip_render();
        SLEEP(0.10);

        ix = (ix + 1) % ARRAY_SIZE(colors);
        tick();
    }
}

void pattern2(void)
{
    start();
    int ixr = 0;
    int ixg = LED_COUNT / 4;
    int ixb = LED_COUNT / 4 * 2;
    int ixy = LED_COUNT / 4 * 3;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set_rgb(ixr, 128, 0, 0);
        strip_set_rgb(ixg, 0, 128, 0);
        strip_set_rgb(ixb, 0, 0, 128);
        strip_set_rgb(ixy, 128, 128, 0);
        strip_render();
        SLEEP(0.1);

        ixr = (ixr + 1) % LED_COUNT;
        ixg = (ixg - 1);
        if (ixg < 0) ixg = LED_COUNT - 1;
        ixb = (ixb + 1) % LED_COUNT;
        ixy = (ixy - 1);
        if (ixy < 0) ixy = LED_COUNT - 1;

        tick();
    }
}

void pattern3(void)
{
    start();
    int x = 0;
    int dx = 1;
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        if (x == 0)
        {
            fill_left(colors[cix]);
        }
        else if (x == WIDTH - 1)
        {
            fill_right(colors[cix]);
        }
        else
        {
            strip_set(top[x - 1], colors[cix]);
            strip_set(bottom[x - 1], colors[cix]);
        }

        strip_render();
        SLEEP(0.10);

        x = x + dx;
        if (x < 0)
        {
            x = 1;
            dx = -dx;
        }

        if (x >= WIDTH)
        {
            x = WIDTH - 2;
            dx = -dx;
        }

        cix = (cix + 1) % ARRAY_SIZE(colors);

        tick();
    }
}

void pattern4(void)
{
    start();
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = 0; i < LED_COUNT; i++)
        {
            float r = (float)rand() / RAND_MAX;
            if (r < 0.2f)
            {
                strip_set(i, colors[cix]);
                cix = (cix + 1) % ARRAY_SIZE(colors);
            }
        }

        strip_render();
        SLEEP(0.2);

        tick();
    }
}

void pattern5(void)
{
    start();
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        fill_top(colors[cix]);
        cix = (cix + 1) % ARRAY_SIZE(colors);
        fill_right(colors[cix]);
        cix = (cix + 1) % ARRAY_SIZE(colors);
        fill_bottom(colors[cix]);
        cix = (cix + 1) % ARRAY_SIZE(colors);
        fill_left(colors[cix]);
        cix = (cix + 1) % ARRAY_SIZE(colors);

        strip_render();
        SLEEP(0.2);

        tick();
    }
}

void pattern6(void)
{
    start();
    int cix = 0;
    int ix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        if (ix == 0)
        {
            fill_top(colors[cix]);
        }

        if (ix == 1)
        {
            fill_right(colors[cix]);
        }

        if (ix == 2)
        {
            fill_bottom(colors[cix]);
        }

        if (ix == 3)
        {
            fill_left(colors[cix]);
        }

        strip_render();

        cix = (cix + 1) % ARRAY_SIZE(colors);
        ix = (ix + 1) % 4;

        SLEEP(0.15);

        tick();
    }
}

void pattern7(void)
{
    start();
    int cix = 0;
    int ix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        fill_top(colors[cix]);
        fill_bottom(colors[cix]);
        strip_render();
        SLEEP(0.15);

        strip_clear();
        fill_right(colors[cix]);
        fill_left(colors[cix]);
        strip_render();
        SLEEP(0.15);

        cix = (cix + 1) % ARRAY_SIZE(colors);

        tick();
    }
}

void pattern8(void)
{
    start();
    int ix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set_rgb((ix + 0) % LED_COUNT, 128, 0, 0);
        strip_set_rgb((ix + 1) % LED_COUNT, 0, 128, 0);
        strip_set_rgb((ix + 2) % LED_COUNT, 0, 0, 128);
        strip_set_rgb((ix + 3) % LED_COUNT, 128, 128, 0);
        strip_set_rgb((ix + 4) % LED_COUNT, 128, 0, 128);
        strip_set_rgb((ix + 5) % LED_COUNT, 0, 128, 128);
        strip_render();
        SLEEP(0.05);
        ix++;

        tick();
    }
}

void pattern9(void)
{
    start();
    int x = 1;
    int dx = 1;
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set(top[x - 1], colors[cix]);
        strip_set(bottom[x - 1], colors[cix]);
        strip_render();

        SLEEP(0.1);

        x = x + dx;
        if (x == 0)
        {
            x = 2;
            dx = -dx;
            cix = (cix + 1) % ARRAY_SIZE(colors);
        }

        if (x >= WIDTH - 2)
        {
            x = WIDTH - 3;
            dx = -dx;
            cix = (cix + 1) % ARRAY_SIZE(colors);
        }

        tick();
    }
}

void pattern10(void)
{
    start();
    int x = 0;
    float ar = 0;
    float ag = M_PI;
    float ab = M_PI / 2.0f;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        int r = ((int)((sin(ar) + 1.0) * 128.0)) % 256;
        int g = ((int)((sin(ag) + 1.0) * 128.0)) % 256;
        int b = ((int)((sin(ab) + 1.0) * 128.0)) % 256;
        strip_set_rgb(x, r / 2, g / 2, b / 2);
        strip_render();

        SLEEP(0.05);

        ar += 0.1f;
        ag += 0.1f;
        ab += 0.1f;
        x = (x + 1) % LED_COUNT;

        tick();
    }
}

void pattern11(void)
{
    start();
    int ix = 0;
    int j = 0;
    int rgb1[] = { 0x00200000, 0x00002000, 0x00000020 };
    int rgb2[] = { 0x00202000, 0x00002020, 0x00200020 };
    int rgb_count = sizeof(rgb1) / sizeof(rgb1[0]);
    assert(sizeof(rgb1) == sizeof(rgb2));

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        for (int i = 0; i < LED_COUNT; ++i)
        {
            if (j % 2 == 0)
                strip_set(i, rgb1[ix]);
            else
                strip_set(i, rgb2[ix]);
            ix = (ix + 1) % rgb_count;
        }

        ix++;
        j++;
        strip_render();
        SLEEP(0.25);

        tick();
    }
}

void pattern12(void)
{
    start();
    int i = 0;
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        i = ((float)rand() / RAND_MAX) * 4;
        strip_clear();
        if (i == 0)
            fill_left(colors[cix]);
        else if (i == 1)
            fill_right(colors[cix]);
        else if (i == 2)
            fill_top(colors[cix]);
        else if (i == 3)
            fill_bottom(colors[cix]);

        strip_render();
        SLEEP(0.15);

        cix = (cix + 1) % ARRAY_SIZE(colors);

        tick();
    }
}

int main(void)
{
    srand(time(NULL));
    printf("Initializing...\n");
    printf("Pin: %d  Leds: %d\n", GPIO_PIN, LED_COUNT);
    if (!neo_init(GPIO_PIN, LED_COUNT))
    {
        return 1;
    }

    int pattern = 0;
    pattern_func *patterns[] = { pattern1, pattern2, pattern3, pattern4, pattern5,
                                 pattern6, pattern7, pattern8, pattern9, pattern10,
                                 pattern11, pattern12 };
    // pattern_func *patterns[] = { pattern12 };
    int pattern_count = sizeof(patterns) / sizeof(patterns[0]);
    printf("%d patterns\n", pattern_count);

    start();
    tick();
    while (!neo_loop_stop())
    {
        printf("Pattern %d ", pattern + 1);
        fflush(stdout);
        (patterns[pattern])();
        pattern = (pattern + 1) % pattern_count;
    }

    printf("\nCleaning up\n");
    neo_deinit();

    return 0;
}
