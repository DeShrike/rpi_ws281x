CC=gcc
HEADERS=clk.h dma.h gpio.h mailbox.h pcm.h pwm.h rpihw.h version.h ws2811.h neopixel.h
OBJS=mailbox.o pcm.o pwm.o rpihw.o dma.o ws2811.o neopixel.o
CFLAGS=-O3

all: retroleds test1 test2 newtest

####################

retroleds: retroleds.o $(OBJS)
	$(CC) $+ -o $@ -lm

retroleds.o: retroleds.c $(HEADERS)
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

newtest: newtest.o $(OBJS)
	$(CC) $+ -o newtest

newtest.o: newtest.c $(HEADERS)
	$(CC) -c $< -o newtest.o $(CFLAGS)

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

clean:
	rm -f -v *.o test1 test2 newtest retroleds

####################

install: retroleds retroleds.service
	sudo cp retroleds /usr/bin
	sudo cp retroleds.service /etc/systemd/system
	sudo systemctl daemon-reload

uninstall: retroleds retroleds.service
	sudo systemctl stop retroleds
	sudo systemctl disable retroleds
	sudo rm /usr/bin/retroleds
	sudo rm /etc/systemd/system/retroleds.service
	sudo systemctl daemon-reload

start:
	sudo systemctl start retroleds

stop:
	sudo systemctl stop retroleds

enable:
	sudo systemctl enable retroleds

disable:
	sudo systemctl disable retroleds
