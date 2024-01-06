CC=gcc
HEADERS=clk.h dma.h gpio.h mailbox.h pcm.h pwm.h rpihw.h version.h ws2811.h neopixel.h
OBJS=mailbox.o pcm.o pwm.o rpihw.o dma.o ws2811.o neopixel.o
CFLAGS=-O0

all: retro test2
# all: test1 test2 test3

####################

retro: retro.o $(OBJS)
	$(CC) $+ -o $@

retro.o: retro.c $(HEADERS)
	$(CC) -c $< -o $@  $(CFLAGS)

####################

test1: test1.o $(OBJS)
	$(CC) $+ -o test1

test1.o: test1.c $(HEADERS)
	$(CC) -c $< -o test1.o $(CFLAGS)

####################

test2: test2.o $(OBJS)
	$(CC) $+ -o test2

test2.o: test2.c $(HEADERS)
	$(CC) -c $< -o test2.o $(CFLAGS)

####################

test3: test3.o $(OBJS)
	$(CC) $+ -o test3

test3.o: test3.c $(HEADERS)
	$(CC) -c $< -o test3.o $(CFLAGS)

####################

mailbox.o: mailbox.c mailbox.h
	$(CC) -c $< -o $@ $(CFLAGS)

pcm.o: pcm.c pcm.h
	$(CC) -c $< -o $@ $(CFLAGS)

dma.o: dma.c dma.h
	$(CC) -c $< -o $@ $(CFLAGS)

pwm.o: pwm.c pwm.h ws2811.h
	$(CC) -c $< -o $@ $(CFLAGS)

rpihw.o: rpihw.c rpihw.h
	$(CC) -c $< -o $@ $(CFLAGS)

ws2811.o: ws2811.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

####################



####################

clean:
	rm -f -v *.o test1 test2 test3 retro
