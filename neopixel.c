#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"
#include "ws2811.h"
#include "neopixel.h"

static ws2811_led_t *strip;

static int led_count;
static int gpio_pin;

static char VERSION[] = "XX.YY.ZZ";
static uint8_t neo_loop_running = 1;
static int CTRL_C_FLAG = 0;  // flag set if ctrl-c is detected

static ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = 0,    // GPIO_PIN,
            .count = 0,      // LED_COUNT,
            .invert = 0,
            .brightness = 64,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

void strip_render(void)
{
    int ret;

    for (int i = 0; i < led_count; i++)
    {
        ledstring.channel[0].leds[i] = strip[i];
    }

    if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
        exit(2);
    }
}

void strip_clear(void)
{
    for (int i = 0; i < led_count; i++)
    {
        strip[i] = 0;
    }
}

void strip_set_rgb(unsigned int index, unsigned char R, unsigned char G, unsigned char B)
{
    if (index >= led_count) return;  // has to be smaller than the number of row

    unsigned long value = (unsigned long) B << 16;
    value |= (unsigned long) G << 8;
    value |= (unsigned long) R;
    strip[index] = value;
}

void strip_set_r(unsigned int index, unsigned char R)
{
    if (index >= led_count) return;  // has to be smaller than the number of row

    unsigned long value = strip[index];
    value |= (unsigned long) R;
    strip[index] = value;
}

void strip_set_g(unsigned int index, unsigned char G)
{
    if (index >= led_count) return;  // has to be smaller than the number of row

    unsigned long value = strip[index];
    value |= (unsigned long) G << 8;
    strip[index] = value;
}

void strip_set_b(unsigned int index, unsigned char B)
{
    if (index >= led_count) return;  // has to be smaller than the number of row

    unsigned long value = strip[index];
    value |= (unsigned long) B << 16;
    strip[index] = value;
}

void strip_set(unsigned int index, uint32_t color)
{
    if (index >= led_count) return;  // has to be smaller than the number of row
    strip[index] = color;
}

void strip_fill_rgb(unsigned char R, unsigned char G, unsigned char B)
{
    unsigned long value = (unsigned long) B  << 16;
    value |= (unsigned long) G  << 8;
    value |= (unsigned long) R;
    for(int index = 0; index < led_count; index++)
    {
        strip[index] = value;
    }
}

void strip_fill(uint32_t color)
{
    for(int index = 0; index < led_count; index++)
    {
        strip[index] = color;
    }
}

void strip_shift_up()
{
    ws2811_led_t temp = strip[led_count - 1];
    for (int i = led_count - 2; i >= 0; i--)
    {
        strip[i + 1] = strip[i];
    }

    strip[0] = temp;
}

void strip_shift_down()
{
    ws2811_led_t temp = strip[0];
    for (int i = 1; i < led_count; i++)
    {
        strip[i - 1] = strip[i];
    }

    strip[led_count - 1] = temp;
}

void neo_sleep(float second)
{
    if (CTRL_C_FLAG) return;  //ctrl-c detected just skip waiting
    usleep((int)(1000000.0 * second));
}

static void ctrl_c_handler(int signum)
{
    CTRL_C_FLAG = 1; //set ctrl-c flag
    (void)(signum);
    neo_loop_running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
}

bool neo_init(int pin, int ledcount)
{
    gpio_pin = pin;
    led_count = ledcount;

    ledstring.channel[0].gpionum = gpio_pin;
    ledstring.channel[0].count = led_count;

    ws2811_return_t ret;

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    strip = malloc(sizeof(ws2811_led_t) * led_count);

    // printf("Setup Handlers\n");
    setup_handlers();

    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return false;
    }
    
    return true;
}

void neo_deinit()
{
    strip_clear();
    strip_render();

    ws2811_fini(&ledstring);

    free(strip);

    printf("\n");
}

bool neo_loop_stop()
{
    return !(bool)neo_loop_running;
}
