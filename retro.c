#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "neopixel.h"

#define GPIO_PIN                18
#define LED_COUNT               30
#define PATTERN_DURATION        15
#define REV(x)                  (LED_COUNT - (x) - 1)

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
        if (x <= 0)
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
        SLEEP(0.1);
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
    int cix3 = (float)rand() / RAND_MAX * colors_count;
    int cix4 = (float)rand() / RAND_MAX * colors_count;

    SLEEP(1);
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        fill_top(colors[cix1]);
        fill_right(colors[cix2]);
        fill_bottom(colors[cix3]);
        fill_left(colors[cix4]);
        strip_render();
        SLEEP(d);

        strip_clear();
        strip_render();
        SLEEP(d);

        d += 0.005f;
        if (d > 0.20f)
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
    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
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
    #define STAR_COUNT 7

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
            star->b = ((float)rand() / RAND_MAX) / 2.0f;
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
        int c = star->b * 128;
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

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION * 2))
    {
        if (state == 0)
        {
            for (int i = 0; i < HORI; ++i)
            {
                cix = (float)rand() / RAND_MAX * colors_count;
                strip_set(top[i], colors[cix]);
            }

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

    strip_set_rgb(left[0], 128, 0, 0);
    strip_set_rgb(left[1], 0, 128, 0);
    strip_set_rgb(left[2], 0, 0, 128);

    strip_set_rgb(right[0], 128, 0, 0);
    strip_set_rgb(right[1], 0, 128, 0);
    strip_set_rgb(right[2], 0, 0, 128);

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
    int ix = 0;
    int colors_count = sizeof(colors) / sizeof(colors[0]);
    int cix = (float)rand() / RAND_MAX * colors_count;

    while (!neo_loop_stop() && (DURATION < PATTERN_DURATION))
    {
        strip_clear();
        strip_set(top[ix], colors[cix]);
        strip_render();
        SLEEP(0.05);

        strip_clear();
        strip_set(bottom[ix], colors[cix]);
        strip_render();
        SLEEP(0.05);

        ix++;
        if (ix >= HORI)
        {
            cix = (float)rand() / RAND_MAX * colors_count;
            ix = 0;
        }

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

    // pattern_func *patterns[] = { pattern24 };
    pattern_func *patterns[] = { pattern1, pattern2, pattern3, pattern4, pattern5,
                                 pattern6, pattern7, pattern8, pattern9, pattern10,
                                 pattern11, pattern12, pattern13, pattern14, pattern15,
                                 pattern16, pattern17, pattern18, pattern19, pattern20,
                                 pattern21, pattern22, pattern23, pattern24 };
    int pattern_count = sizeof(patterns) / sizeof(patterns[0]);

    // Shuffle the patterns
    for (int i = 0; i < 1000; ++i)
    {
        int a = rand() % pattern_count;
        int b = rand() % pattern_count;
        pattern_func *temp = patterns[a];
        patterns[a] = patterns[b];
        patterns[b] = temp;
    }

    printf("%d patterns\n", pattern_count);

    start();
    tick();
    while (!neo_loop_stop())
    {
        printf("\rPattern %d ", pattern + 1);
        fflush(stdout);
        (patterns[pattern])();
        pattern = (pattern + 1) % pattern_count;
    }

    printf("\nCleaning up\n");
    neo_deinit();

    return 0;
}
