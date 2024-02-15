#include <assert.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "neopixel.h"

///////////////////////////////////////////////////////////////////////
// For RetroPie, use PCM pin 21 !!!!
// SPI and PWM do not work together with the sound setup of RetroPie.
///////////////////////////////////////////////////////////////////////

// SPI: Pin 10
#define GPIO_PIN                10

// PWM: Pin 18
//#define GPIO_PIN                18

// PWM: Pin 12
//#define GPIO_PIN                12

// PCM: Pin 21
//#define GPIO_PIN                21

// PCM: Pin 31
//#define GPIO_PIN                31

#define LED_COUNT               40
#define PATTERN_DURATION        15
#define REV(x)                  (LED_COUNT - (x) - 1)
#define MAX_BRIGHTNESS           100

typedef void pattern_func(void);

uint32_t colors[] =
{
    0x00200000,  // red
    0x00201000,  // orange
    0x00200010,  // pink
    0x00202000,  // yellow
    0x00002000,  // green
    0x00000020,  // blue
    0x00100010,  // purple
    0x00202020,  // white
    0x00002020,  // lightblue
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

/*

Strip layout:

 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16
39                                                 17
38                                                 18
37                                                 19
 36 35 34 33 32 31 30 29 28 27 26 25 24 23 22 21 20

*/

unsigned int top[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
unsigned int bottom[17] = { 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20 };
unsigned int right[3] = { 17, 18, 19 };
unsigned int left[3] = { 37, 38, 39 };

unsigned int subs[8][5] = {
    { 1, 2, 3, 4, 5 },
    { 6, 7, 8, 9, 10 },
    { 11, 12, 13, 14, 15 },
    { 16, 17, 18, 19, 20 },
    { 21, 22, 23, 24, 25 },
    { 26, 27, 28, 29, 30 },
    { 31, 32, 33, 34, 35 },
    { 36, 37, 38, 39, 0 }
};

//#define SLEEP sleep_ms
#define SLEEP neo_sleep

#define HORI   (sizeof(top) / sizeof(int))
#define VERT   (sizeof(left) / sizeof(int))
#define WIDTH  (HORI + 2)
#define HEIGHT (VERT + 2)

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

////////////////////////////////////////////////////////////////////////////////
// Patterns Start Here /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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
        strip_set_rgb(ixr, MAX_BRIGHTNESS, 0, 0);
        strip_set_rgb(ixg, 0, MAX_BRIGHTNESS, 0);
        strip_set_rgb(ixb, 0, 0, MAX_BRIGHTNESS);
        strip_set_rgb(ixy, MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
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
        SLEEP(0.15);

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
        strip_set_rgb((ix + 0) % LED_COUNT, MAX_BRIGHTNESS, 0, 0);
        strip_set_rgb((ix + 1) % LED_COUNT, 0, MAX_BRIGHTNESS, 0);
        strip_set_rgb((ix + 2) % LED_COUNT, 0, 0, MAX_BRIGHTNESS);
        strip_set_rgb((ix + 3) % LED_COUNT, MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
        strip_set_rgb((ix + 4) % LED_COUNT, MAX_BRIGHTNESS, 0, MAX_BRIGHTNESS);
        strip_set_rgb((ix + 5) % LED_COUNT, 0, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
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

        if (x >= WIDTH - 1)
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

        ix = (ix + 1) % rgb_count;
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

void pattern13(void)
{
    start();
    int x0 = 0;
    int dx = 1;
    int x1 = 0;
    int x2 = 0;
    int x3 = 0;
    int x4 = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set_rgb(top[x4], 5, 0, 0);
        strip_set_rgb(bottom[x4], 5, 0, 0);

        strip_set_rgb(top[x3], 30, 0, 0);
        strip_set_rgb(bottom[x3], 30, 0, 0);

        strip_set_rgb(top[x2], 80, 0, 0);
        strip_set_rgb(bottom[x2], 80, 0, 0);

        strip_set_rgb(top[x1], 100, 0, 0);
        strip_set_rgb(bottom[x1], 100, 0, 0);

        strip_set_rgb(top[x0], 128, 0, 0);
        strip_set_rgb(bottom[x0], 128, 0, 0);

        strip_render();
        SLEEP(0.1);

        x4 = x3;
        x3 = x2;
        x2 = x1;
        x1 = x0;

        x0 = x0 + dx;
        if (x0 < 0)
        {
            x0 = 1;
            dx = -dx;
        }

        if (x0 >= HORI)
        {
            x0 = HORI - 2;
            dx = -dx;
        }

        tick();
    }
}

void pattern14(void)
{
    start();
    int x = 0;
    int cix = 0;
    strip_clear();
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_set(top[x], colors[cix]);
        strip_set(bottom[x], colors[cix]);
        strip_render();

        SLEEP(0.1);
        x++;
        if (x >= HORI)
        {
            x = 0;
            cix = (cix + 1) % ARRAY_SIZE(colors);
        }

        tick();
    }
}

void pattern15(void)
{
    start();
    int x = 0;
    int dx = 1;
    int cix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set(top[x], colors[cix]);
        strip_set(bottom[HORI - x - 1], colors[cix]);
        strip_render();

        SLEEP(0.05);
        x = x + dx;
        if (x < 0)
        {
            x = 1;
            dx = -dx;
            cix = (cix + 1) % ARRAY_SIZE(colors);
        }

        if (x >= HORI)
        {
            x = HORI - 2;
            dx = -dx;
            cix = (cix + 1) % ARRAY_SIZE(colors);
        }

        tick();
    }
}

void pattern16(void)
{
    start();
    int x = 0;
    int dx = 10;
    int i = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        if (i == 0)
            strip_fill_rgb(x, 0, 0);
        else if (i == 1)
            strip_fill_rgb(0, x, 0);
        else
            strip_fill_rgb(0, 0, x);
        strip_render();

        SLEEP(0.05);
        x = x + dx;
        if (x <= 0)
        {
            x = 20;
            dx = -dx;
            i = (i + 1) % 3;
        }

        if (x >= 140)
        {
            x = 120;
            dx = -dx;
        }

        tick();
    }
}

void pattern17(void)
{
    start();
    int ix = 0;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set_rgb((ix + 0) % LED_COUNT, 0, 0, 20);
        strip_set_rgb((ix + 1) % LED_COUNT, 40, 0, 0);
        strip_set_rgb((ix + 2) % LED_COUNT, 60, 0, 0);
        strip_set_rgb((ix + 3) % LED_COUNT, 80, 0, 0);
        strip_set_rgb((ix + 4) % LED_COUNT, 100, 0, 0);
        strip_set_rgb((ix + 5) % LED_COUNT, 120, 0, 0);
        strip_set_rgb((ix + 6) % LED_COUNT, 0, 140, 0);
        strip_render();
        SLEEP(0.05);
        ix++;

        tick();
    }
}

void pattern18(void)
{
    start();
    float d = 0.01f;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix1 = (float)rand() / RAND_MAX * colors_count;
    int cix2 = (float)rand() / RAND_MAX * colors_count;
    while (cix2 == cix1)
        cix2 = (float)rand() / RAND_MAX * colors_count;

    int cix3 = (float)rand() / RAND_MAX * colors_count;
    while (cix3 == cix1 || cix3 == cix2)
        cix3 = (float)rand() / RAND_MAX * colors_count;

    int cix4 = (float)rand() / RAND_MAX * colors_count;
    while (cix4 == cix1 || cix4 == cix2 || cix4 == cix3)
        cix4 = (float)rand() / RAND_MAX * colors_count;

    SLEEP(1);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        fill_top(colors[cix1]);
        fill_right(colors[cix2]);
        fill_bottom(colors[cix3]);
        fill_left(colors[cix4]);
        strip_render();
        SLEEP(d * d);

        strip_clear();
        strip_render();
        SLEEP(d * d);

        d += 0.005f;
        if (d > 0.50f)
        {
            break;
        }

        tick();
    }

    SLEEP(1);
}

void pattern19(void)
{
    start();
    int i = 0;
    int m = LED_COUNT;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix = (float)rand() / RAND_MAX * colors_count;

    SLEEP(0.5);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        strip_clear();
        strip_set(REV(i), colors[cix]);

        for (int j = m; j <= LED_COUNT; j++)
        {
            strip_set(REV(j), colors[cix]);
        }

        strip_render();
        SLEEP(0.02);

        i = i + 1;
        if (i == m)
        {
            i = 0;
            m -= 1;
            if (m <= 0)
            {
                break;
            }
        }

        tick();
    }

    SLEEP(0.5);
    strip_clear();
    strip_render();
    SLEEP(0.5);
}

void pattern20(void)
{
    #define STAR_COUNT (LED_COUNT / 3)

    struct star {
        int pos;
        int rgb;
        float b;
        float deltab;
    };

    void makestar(struct star *star, bool r)
    {
        star->deltab = 0.075f;
        star->rgb = (float)rand() / RAND_MAX * 4;
        star->pos = (float)rand() / RAND_MAX * LED_COUNT;
        if (r)
        {
            star->b = ((float)rand() / RAND_MAX);
        }
        else
        {
            star->b = 0.0f;
        }
    }

    void updatestar(struct star *star)
    {
        star->b += star->deltab;
        if (star->b > 1.0f)
        {
            star->b = 1.0f;
            star->deltab *= -1;
        }

        if (star->b < 0.0f)
        {
            makestar(star, false);
        }
    }

    void drawstar(struct star *star)
    {
        int c = star->b * MAX_BRIGHTNESS;
        if (star->rgb == 0)
        {
            strip_set_rgb(star->pos, c, 0, 0);
        }
        else if (star->rgb == 1)
        {
            strip_set_rgb(star->pos, 0, c, 0);
        }
        else if (star->rgb == 2)
        {
            strip_set_rgb(star->pos, 0, 0, c);
        }
        else
        {
            strip_set_rgb(star->pos, c, c, 0);
        }
    }

    start();

    struct star stars[STAR_COUNT];
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        makestar(&stars[i], true);
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        strip_clear();
        for (int i = 0; i < STAR_COUNT; i++)
        {
            drawstar(&stars[i]);
            updatestar(&stars[i]);
        }

        strip_render();
        SLEEP(0.1);

        tick();
    }
}

void pattern21(void)
{
    start();

    SLEEP(0.5);

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

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_render();
        SLEEP(0.05);

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

        tick();
    }

    SLEEP(0.5);
}

void pattern22(void)
{
    start();

    SLEEP(0.5);
    strip_clear();

    int cix;
    int colors_count = sizeof(colors) / sizeof(colors[0]);

    int state = 0;
    int shifts = 0;

    for (int i = 0; i < HORI; ++i)
    {
        cix = (float)rand() / RAND_MAX * colors_count;
        strip_set(top[i], colors[cix]);
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        if (state == 0)
        {
            strip_render();
            state = 1;
            shifts = HORI + VERT;
            SLEEP(0.5);
        }
        else if (state == 1)
        {
            strip_shift_down();
            strip_render();
            SLEEP(0.025);
            shifts--;
            if (shifts == 0)
            {
                state = 2;
            }
        }
        else if (state == 2)
        {
            SLEEP(0.5);
            shifts = HORI + VERT;
            state = 3;
        }
        else if (state == 3)
        {
            strip_shift_down();
            strip_render();
            SLEEP(0.025);
            shifts--;
            if (shifts == 0)
            {
                state = 0;
            }
        }

        tick();
    }

    SLEEP(0.5);
}

void pattern23(void)
{
    start();

    SLEEP(0.5);
    strip_clear();

    strip_set_rgb(left[0], MAX_BRIGHTNESS, 0, 0);
    strip_set_rgb(left[1], 0, MAX_BRIGHTNESS, 0);
    strip_set_rgb(left[2], 0, 0, MAX_BRIGHTNESS);

    strip_set_rgb(right[0], MAX_BRIGHTNESS, 0, 0);
    strip_set_rgb(right[1], 0, MAX_BRIGHTNESS, 0);
    strip_set_rgb(right[2], 0, 0, MAX_BRIGHTNESS);

    int state = 0;
    int shifts = 0;

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        if (state == 0)
        {
            strip_render();
            state = 1;
            shifts = HORI + VERT;
            SLEEP(0.25);
        }
        else if (state == 1)
        {
            strip_shift_up();
            strip_render();
            SLEEP(0.025);
            shifts--;
            if (shifts == 0)
            {
                state = 2;
            }
        }
        else if (state == 2)
        {
            SLEEP(0.25);
            shifts = HORI + VERT;
            state = 3;
        }
        else if (state == 3)
        {
            strip_shift_up();
            strip_render();
            SLEEP(0.025);
            shifts--;
            if (shifts == 0)
            {
                state = 0;
            }
        }

        tick();
    }

    SLEEP(0.5);
}

void pattern24(void)
{
    start();
    int ix1 = 0;
    int ix2 = LED_COUNT / 2;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix1 = (float)rand() / RAND_MAX * colors_count;
    int cix2 = (float)rand() / RAND_MAX * colors_count;

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set(top[ix1], colors[cix1]);
        strip_set(REV(top[ix1]), colors[cix1]);
        strip_set(top[ix2], colors[cix1]);
        strip_set(REV(top[ix2]), colors[cix2]);
        strip_render();
        SLEEP(0.01);

        strip_clear();
        strip_set(bottom[ix1], colors[cix1]);
        strip_set(REV(bottom[ix1]), colors[cix1]);
        strip_set(bottom[ix2], colors[cix2]);
        strip_set(REV(bottom[ix2]), colors[cix2]);
        strip_render();
        SLEEP(0.05);

        ix1++;
        if (ix1 >= HORI)
        {
            cix1 = (float)rand() / RAND_MAX * colors_count;
            ix1 = 0;
        }

        ix2--;
        if (ix2 <0)
        {
            cix2 = (float)rand() / RAND_MAX * colors_count;
            ix2 = LED_COUNT;
        }

        tick();
    }
}

void pattern25(void)
{
    start();

    SLEEP(0.5);

    strip_clear();
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    for (int i = 0; i < LED_COUNT; i += 4)
    {
        int cix = (float)rand() / RAND_MAX * colors_count;
        strip_set(i, colors[cix]);
    }

    time_t t;
    srand((unsigned) time(&t));
    int direction = 1;

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_render();
        SLEEP(0.05);

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

        tick();
    }

    SLEEP(0.5);
}

void pattern26(void)
{
    start();

    SLEEP(0.5);

    strip_clear();
    int colors_count = sizeof(colors) / sizeof(colors[0]);

    time_t t;
    srand((unsigned) time(&t));
    int dx1 = 1;
    int dx2 = 1;
    int dx3 = 1;
    int x1 = LED_COUNT / 4 * 1;
    int x2 = LED_COUNT / 4 * 2;
    int x3 = LED_COUNT / 4 * 3;
    int cols[LED_COUNT] = {0};
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        x1 = (x1 + dx1 + LED_COUNT) % LED_COUNT;
        cols[x1]++;
        x2 = (x2 + dx2 + LED_COUNT) % LED_COUNT;
        cols[x2]++;
        x3 = (x3 + dx3 + LED_COUNT) % LED_COUNT;
        cols[x3]++;

        if (cols[x1] >= colors_count)
        {
            cols[x1] = -1;
        }

        if (cols[x2] >= colors_count)
        {
            cols[x2] = -1;
        }

        if (cols[x3] >= colors_count)
        {
            cols[x3] = -1;
        }

        if (cols[x1] == -1)
        {
            strip_set_rgb(x1, 0, 0, 0);
        }
        else
        {
            strip_set(x1, colors[cols[x1]]);
        }

        if (cols[x2] == -1)
        {
            strip_set_rgb(x2, 0, 0, 0);
        }
        else
        {
            strip_set(x2, colors[cols[x2]]);
        }

        if (cols[x3] == -1)
        {
            strip_set_rgb(x3, 0, 0, 0);
        }
        else
        {
            strip_set(x3, colors[cols[x3]]);
        }

        strip_render();
        SLEEP(0.05);

        if ((rand() % 1000) < 500)
        {
            dx1 *= -1;
        }

        if ((rand() % 1000) < 500)
        {
            dx2 *= -1;
        }

        if ((rand() % 1000) < 500)
        {
            dx3 *= -1;
        }

        tick();
    }

    SLEEP(0.5);
}

void pattern27(void)
{
    start();

    SLEEP(0.5);

    strip_clear();
    int r = 128;
    int x = 0;
    while (r > 0)
    {
        strip_set_rgb(x, 0, r, 0);
        r -= 5;
        x++;
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_render();
        SLEEP(0.05);
        strip_shift_down();
        tick();
    }

    SLEEP(0.5);
}

void pattern28(void)
{
    start();

    SLEEP(0.5);

    strip_clear();
    int r = 128;
    int x = 0;
    while (r > 0)
    {
        strip_set_rgb(x, 0, 0, r);
        r -= 5;
        x++;
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_render();
        SLEEP(0.05);
        strip_shift_down();
        tick();
    }

    SLEEP(0.5);
}

void pattern29(void)
{
    start();

    SLEEP(0.5);

    strip_clear();
    int r = 0;
    int x = 0;
    while (r <= 128)
    {
        strip_set_rgb(x, r, 0, 0);
        r += 5;
        x++;
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_render();
        SLEEP(0.05);
        strip_shift_up();
        tick();
    }

    SLEEP(0.5);
}

void pattern30(void)
{
    start();

    SLEEP(0.5);

    int x1 = LED_COUNT / 4 * 1;
    int x3 = LED_COUNT / 4 * 2;
    int x2 = LED_COUNT / 4 * 3;
    int counter = 0;
    int x;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        strip_clear();
        for (int i = 0; i < 5; i++)
        {
            x = (x1 + i) % LED_COUNT;
            strip_set_r(x, MAX_BRIGHTNESS);
            x = (x2 + i) % LED_COUNT;
            strip_set_g(x, MAX_BRIGHTNESS);
            x = (x3 + i) % LED_COUNT;
            strip_set_b(x, MAX_BRIGHTNESS);
        }

        strip_render();

        counter++;
        x1 = (x1 + 1) % LED_COUNT;
        if (counter % 2 == 1)
        {
            x2 = (x2 + 1) % LED_COUNT;
        }

        if (counter % 3 == 1)
        {
            x3 = (x3 - 1 + LED_COUNT) % LED_COUNT;
        }

        SLEEP(0.05);
        tick();
    }

    SLEEP(0.5);
}

void pattern31(void)
{
    void setcenter(int count)
    {
        if (count % 2 == 0)
        {
            strip_set_rgb(left[1], 50, 50, 50);
        }
        else
        {
            strip_set_rgb(right[1], 50, 50, 50);
        }
    }

    start();

    int count1 = 0;
    int count2 = 0;
    SLEEP(0.5);
    float d = 0.03f;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = 0; i < HORI / 2; i++)
        {
            strip_set_rgb(bottom[i], 0, MAX_BRIGHTNESS, 0);
        }

        setcenter(count1);

        strip_render();
        SLEEP(d);

        strip_clear();
        for (int i = 0; i < HORI / 2; i++)
        {
            strip_set_rgb(top[i], 0, 0, MAX_BRIGHTNESS);
        }

        setcenter(count1);

        strip_render();
        SLEEP(d);

        strip_clear();
        for (int i = HORI / 2; i < HORI; i++)
        {
            strip_set_rgb(bottom[i], MAX_BRIGHTNESS, 0, 0);
        }

        setcenter(count1);

        strip_render();
        SLEEP(d);

        strip_clear();
        for (int i = HORI / 2; i < HORI; i++)
        {
            strip_set_rgb(top[i], MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
        }

        setcenter(count1);

        strip_render();
        SLEEP(d);

        tick();
        count2++;
        if (count2 == 5)
        {
            count1++;
            count2 = 0;
        }
    }

    strip_clear();
    strip_render();

    SLEEP(0.5);
}

void pattern32(void)
{
    start();

    int count1 = 0;
    int count2 = 0;
    SLEEP(0.5);
    float d = 0.04f;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        if (count1 % 2 == 0)
        {
            for (int i = 0; i < VERT; i++)
            {
                strip_set_rgb(left[i], 0, 0, MAX_BRIGHTNESS);
            }

            for (int i = 0; i < HORI / 2; i++)
            {
                strip_set_rgb(bottom[i], 0, 0, MAX_BRIGHTNESS);
                strip_set_rgb(top[i], 0, 0, MAX_BRIGHTNESS);
            }
        }
        else
        {
            for (int i = 0; i < VERT; i++)
            {
                strip_set_rgb(right[i], 0, 0, MAX_BRIGHTNESS);
            }

            for (int i = HORI / 2; i < HORI; i++)
            {
                strip_set_rgb(bottom[i], 0, 0, MAX_BRIGHTNESS);
                strip_set_rgb(top[i], 0, 0, MAX_BRIGHTNESS);
            }
        }

        strip_render();
        SLEEP(d);

        strip_clear();
        strip_render();
        SLEEP(d);

        tick();
        count2++;
        if (count2 == 5)
        {
            count1++;
            count2 = 0;
        }
    }

    strip_clear();
    strip_render();

    SLEEP(0.5);
}

void pattern33(void)
{
    start();

    int count1 = 0; // 0 to 3
    int count2 = 0;
    SLEEP(0.1);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = count1; i < LED_COUNT; i += 4)
        {
            if (count1 == 0)
            {
                strip_set_rgb(i, MAX_BRIGHTNESS, 0, 0);
            }
            else if (count1 == 1)
            {
                strip_set_rgb(i, 0, MAX_BRIGHTNESS, 0);
            }
            else if (count1 == 2)
            {
                strip_set_rgb(i, 0, 0, MAX_BRIGHTNESS);
            }
            else if (count1 == 3)
            {
                strip_set_rgb(i, MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
            }
        }

        count2++;
        if (count2 == 3)
        {
            count1 = (count1 + 1) % 4;
            count2 = 0;
        }

        strip_render();
        SLEEP(0.05);

        strip_clear();
        strip_render();
        SLEEP(0.1);

        tick();
    }

    strip_clear();
    strip_render();

    SLEEP(0.5);
}

void pattern34(void)
{
    start();

    int count1 = 0; // 0 to 3
    SLEEP(0.1);
    int c = 0;
    int dc = 10;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = count1; i < LED_COUNT; i += 4)
        {
            if (count1 == 0)
            {
                strip_set_rgb(i, c, 0, 0);
            }
            else if (count1 == 1)
            {
                strip_set_rgb(i, 0, c, 0);
            }
            else if (count1 == 2)
            {
                strip_set_rgb(i, 0, 0, c);
            }
            else if (count1 == 3)
            {
                strip_set_rgb(i, c, c, 0);
            }
        }

        c = c + dc;
        if (c > 128)
        {
            dc = -dc;
        }
        else if (c < 0)
        {
            count1 = (count1 + 1) % 4;
            c = 0;
            dc = 10;
        }

        strip_render();
        SLEEP(0.05);

        tick();
    }

    strip_clear();
    strip_render();

    SLEEP(0.5);
}

void pattern35(void)
{
    start();

    int speed = 15;
    SLEEP(0.1);
    int r = 0;
    int g = 50;
    int b = 50;
    int y = 100;
    int dr = speed;
    int dg = -speed;
    int db = speed;
    int dy = -speed;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = 0; i < LED_COUNT; i += 4)
        {
            strip_set_rgb(i, r, 0, 0);
        }

        for (int i = 1; i < LED_COUNT; i += 4)
        {
            strip_set_rgb(i, 0, g, 0);
        }

        for (int i = 2; i < LED_COUNT; i += 4)
        {
            strip_set_rgb(i, 0, 0, b);
        }

        for (int i = 3; i < LED_COUNT; i += 4)
        {
            strip_set_rgb(i, y, y, 0);
        }

        r = r + dr;
        if (r > 128)
        {
            dr = -dr;
        }
        else if (r < 0)
        {
            r = 0;
            dr = speed;
        }

        g = g + dg;
        if (g > 128)
        {
            dg = -dg;
        }
        else if (g < 0)
        {
            g = 0;
            dg = speed;
        }

        b = b + db;
        if (b > 128)
        {
            db = -db;
        }
        else if (b < 0)
        {
            b = 0;
            db = speed;
        }

        y = y + dy;
        if (y > 128)
        {
            dy = -dy;
        }
        else if (y < 0)
        {
            y = 0;
            dy = speed;
        }

        strip_render();
        SLEEP(0.05);

        tick();
    }

    strip_clear();
    strip_render();

    SLEEP(0.5);
}

void pattern36(void)
{
    struct star {
        int startDelay;
        int pos;
        int rgb;
        float b;
        float deltab;
    };

    void makestar(struct star *star, int pos)
    {
        star->b = 0;
        star->deltab = rand() % 12 + 7;
        star->startDelay = rand() % 10 + 3;
        star->rgb = rand() % 6;
        star->pos = pos;
    }

    void resetstar(struct star *star)
    {
        star->b = 0;
        star->deltab = -star->deltab;
        star->rgb = rand() % 6;
        star->deltab = rand() % 12 + 7;
        star->startDelay = rand() % 10 + 3;
    }

    void updatestar(struct star *star)
    {
        if (star->startDelay > 0)
        {
            star->startDelay--;
        }
        else
        {
            star->b += star->deltab;
            if (star->b > 128)
            {
                star->deltab = -star->deltab;
            }

            if (star->b < 0)
            {
                resetstar(star);
            }
        }
    }

    void drawstar(struct star *star)
    {
        int c = star->b;
        if (star->rgb == 0)
        {
            strip_set_rgb(star->pos, c, 0, 0);
        }
        else if (star->rgb == 1)
        {
            strip_set_rgb(star->pos, 0, c, 0);
        }
        else if (star->rgb == 2)
        {
            strip_set_rgb(star->pos, 0, 0, c);
        }
        else if (star->rgb == 3)
        {
            strip_set_rgb(star->pos, 0, c, c);
        }
        else if (star->rgb == 4)
        {
            strip_set_rgb(star->pos, c, 0, c);
        }
        else if (star->rgb == 5)
        {
            strip_set_rgb(star->pos, c, c, 0);
        }
    }

    start();

    struct star stars[LED_COUNT];
    for (int i = 0; i < LED_COUNT; ++i)
    {
        makestar(&stars[i], i);
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        strip_clear();
        for (int i = 0; i < LED_COUNT; i++)
        {
            drawstar(&stars[i]);
            updatestar(&stars[i]);
        }

        strip_render();
        SLEEP(0.1);

        tick();
    }
}

void pattern37(void)
{
    start();
    int ixr = 0;
    int ixg = LED_COUNT / 6;
    int ixb = LED_COUNT / 6 * 2;
    int ixy = LED_COUNT / 6 * 3;
    int ixc = LED_COUNT / 6 * 4;
    int ixm = LED_COUNT / 6 * 5;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set_rgb(ixr, MAX_BRIGHTNESS, 0, 0);
        strip_set_rgb(ixg, 0, MAX_BRIGHTNESS, 0);
        strip_set_rgb(ixb, 0, 0, MAX_BRIGHTNESS);
        strip_set_rgb(ixy, MAX_BRIGHTNESS, MAX_BRIGHTNESS, 0);
        strip_set_rgb(ixc, MAX_BRIGHTNESS, 0, MAX_BRIGHTNESS);
        strip_set_rgb(ixm, 0, MAX_BRIGHTNESS, MAX_BRIGHTNESS);
        strip_render();
        SLEEP(0.1);

        ixr = (ixr + 1) % LED_COUNT;
        ixg = (ixg - 1);
        if (ixg < 0) ixg = LED_COUNT - 1;
        ixb = (ixb + 1) % LED_COUNT;
        ixy = (ixy - 1);
        if (ixy < 0) ixy = LED_COUNT - 1;
        ixc = (ixc + 1) % LED_COUNT;
        ixm = (ixm - 1);
        if (ixm < 0) ixm = LED_COUNT - 1;

        tick();
    }
}

void pattern38(void)
{
    start();
    int six = rand() % 8;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        int nsix = rand() % 8;
        while (six == nsix)
        {
            nsix = rand() % 8;
        }

        six = nsix;

        int cix = (float)rand() / RAND_MAX * colors_count;
        for (int i = 0; i < 5; i++)
        {
            strip_set(subs[six][i], colors[cix]);
        }

        strip_render();
        SLEEP(0.1);
        tick();
    }
}

void pattern39(void)
{
    start();
    strip_clear();
    int six = rand() % 8;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        int nsix = rand() % 8;
        while (six == nsix)
        {
            nsix = rand() % 8;
        }

        six = nsix;

        int cix = (float)rand() / RAND_MAX * colors_count;
        for (int i = 1; i < 5; i++)
        {
            strip_set(subs[six][i], colors[cix]);
        }

        strip_render();
        SLEEP(0.1);
        tick();
    }
}

void pattern40(void)
{
    struct star {
        int startDelay;
        int pos;
        int rgb;
        float b;
        float deltab;
    };

    void makestar(struct star *star, int pos)
    {
        star->b = 0;
        star->deltab = rand() % 12 + 7;
        star->startDelay = rand() % 10 + 3;
        star->rgb = rand() % 6;
        star->pos = pos;
    }

    void resetstar(struct star *star)
    {
        star->b = 0;
        star->deltab = -star->deltab;
        star->rgb = rand() % 6;
        star->deltab = rand() % 12 + 7;
        star->startDelay = rand() % 10 + 3;
    }

    void updatestar(struct star *star)
    {
        if (star->startDelay > 0)
        {
            star->startDelay--;
        }
        else
        {
            star->b += star->deltab;
            if (star->b > 128)
            {
                star->deltab = -star->deltab;
            }

            if (star->b < 0)
            {
                resetstar(star);
            }
        }
    }

    void drawstar(struct star *star)
    {
        int c = star->b;
        if (star->rgb == 0)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], c, 0, 0);
            }
        }
        else if (star->rgb == 1)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], 0, c, 0);
            }
        }
        else if (star->rgb == 2)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], 0, 0, c);
            }
        }
        else if (star->rgb == 3)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], 0, c, c);
            }
        }
        else if (star->rgb == 4)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], c, 0, c);
            }
        }
        else if (star->rgb == 5)
        {
            for (int i = 0; i < 5; i++)
            {
                strip_set_rgb(subs[star->pos][i], c, c, 0);
            }
        }
    }

    start();

    struct star stars[8];
    for (int i = 0; i < 8; ++i)
    {
        makestar(&stars[i], i);
    }

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        strip_clear();
        for (int i = 0; i < 8; i++)
        {
            drawstar(&stars[i]);
            updatestar(&stars[i]);
        }

        strip_render();
        SLEEP(0.1);

        tick();
    }
}

void pattern41(void)
{
    start();
    strip_clear();
    int six1 = 0;
    int six2 = 4;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix1 = (float)rand() / RAND_MAX * colors_count;
    int cix2 = (float)rand() / RAND_MAX * colors_count;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int i = 0; i < 5; i++)
        {
            strip_set(subs[six1][i], colors[cix1]);
            strip_set(subs[six2][i], colors[cix2]);
        }

        strip_render();

        six1 = (six1 + 1) % 8;
        six2 = (six2 + 1) % 8;
        SLEEP(0.1);
        tick();
    }
}

void pattern42(void)
{
    start();
    strip_clear();
    int c = HORI / 2;
    int m = c + 1;
    int i = 0;
    int di = 1;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix1 = (float)rand() / RAND_MAX * colors_count;
    int cix2 = (float)rand() / RAND_MAX * colors_count;
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        for (int x = 0; x < i; x++)
        {
            strip_set(top[c - x], colors[cix1]);
            strip_set(top[c + x], colors[cix1]);
            strip_set(bottom[c - x], colors[cix2]);
            strip_set(bottom[c + x], colors[cix2]);
        }

        strip_render();

        i = i + di;
        if (i < 0)
        {
            i = 0;
            di = 1;
            cix1 = (float)rand() / RAND_MAX * colors_count;
            cix2 = (float)rand() / RAND_MAX * colors_count;
        }

        if (i > m)
        {
            i = m;
            di = -1;
        }

        SLEEP(0.02);
        tick();
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

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

    // pattern_func *patterns[] = { pattern18, pattern24, pattern42 };
    pattern_func *patterns[] = { pattern1, pattern2, pattern3, pattern4, pattern5,
                                 pattern6, pattern7, pattern8, pattern9, pattern10,
                                 pattern11, pattern12, pattern13, pattern14, pattern15,
                                 pattern16, pattern17, pattern18, pattern19, pattern20,
                                 pattern21, pattern22, pattern23, pattern24, pattern25,
                                 pattern26, pattern27, pattern28, pattern29, pattern30,
                                 pattern31, pattern32, pattern33, pattern34, pattern35,
                                 pattern36, pattern37, pattern38, pattern39, pattern40,
                                 pattern41, pattern42 };
    int pattern_count = sizeof(patterns) / sizeof(patterns[0]);

    int* orders = malloc(pattern_count * sizeof(int));
    for (int i = 0; i < pattern_count; ++i)
    {
        orders[i] = i;
    }

    // Shuffle the patterns
    for (int i = 0; i < 1000; ++i)
    {
        int a = rand() % pattern_count;
        int b = rand() % pattern_count;
        int temp = orders[a];
        orders[a] = orders[b];
        orders[b] = temp;
    }

    printf("%d patterns\n", pattern_count);

    start();
    tick();
    while (!neo_loop_stop())
    {
        printf("\rPattern %d ", orders[pattern] + 1);
        fflush(stdout);
        (patterns[orders[pattern]])();
        pattern = (pattern + 1) % pattern_count;
    }

    free(orders);

    strip_clear();
    for (int i = 0; i < LED_COUNT; i += 4)
    {
        if (received_signal == SIGINT)
            strip_set_rgb(i, 0, 4, 0);
        else if (received_signal == SIGTERM)
            strip_set_rgb(i, 4, 0, 0);
        else if (received_signal == SIGHUP)
            strip_set_rgb(i, 0, 0, 4);
    }

    strip_render();

    printf("\nCleaning up\n");
    neo_deinit(false);

    return 0;
}
