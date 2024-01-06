#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>

#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

#include "ws2811.h"

#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)
#define LED_COUNT               7

static char VERSION[] = "XX.YY.ZZ";

int led_count = LED_COUNT;
int clear_on_exit = 1;

ws2811_led_t *strip;

static uint8_t running = 1;
int CTRL_C_FLAG = 0;  // flag set if ctrl-c is detected

ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 127,
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

void strip_set(unsigned int index, unsigned char R, unsigned char G, unsigned char B)
{
    if (index >= LED_COUNT) return;  // has to be smaller than the number of row

    unsigned long value = (unsigned long) B << 16;
    value |= (unsigned long) G << 8;
    value |= (unsigned long) R;
    strip[index] = value;
}

void strip_fill(unsigned char R, unsigned char G, unsigned char B)
{
    unsigned long value = (unsigned long) B  << 16;
    value |= (unsigned long) G  << 8;
    value |= (unsigned long) R  ;
    for(int index = 0; index < led_count; index++)
    {
        strip[index] = value;
    }
}

void mysleep(float second)
{
    if (CTRL_C_FLAG) return;  //ctrl-c detected just skip waiting
    usleep((int)(1000000.0 * second));
}

static void ctrl_c_handler(int signum)
{
    CTRL_C_FLAG = 1; //set ctrl-c flag
	(void)(signum);
    running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa =
    {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main(int argc, char *argv[])
{
    ws2811_return_t ret;

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    strip = malloc(sizeof(ws2811_led_t) * led_count);

    printf("Setup Handlers\n");
    setup_handlers();

    printf("Initializing...\n");
    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return ret;
    }

    strip[0] = 0x00200000;  // red
    strip[0] = 0x00201000;  // orange
    strip[1] = 0x00202000;  // yellow
    strip[2] = 0x00002000;  // green
    strip[3] = 0x00002020;  // lightblue
    strip[4] = 0x00000020;  // blue
    strip[5] = 0x00100010;  // purple
    strip[6] = 0x00200010;  // pink

    time_t t;
    srand((unsigned) time(&t));
    int direction = 1;
    printf("Starting loop\n");
    while (running)
    {
        strip_render();
        mysleep(0.10);
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

    if (clear_on_exit)
    {
        strip_clear();
        strip_render();
    }

    printf("Cleaning up\n");
    ws2811_fini(&ledstring);

    free(strip);
    printf("\n");
    return ret;
}
