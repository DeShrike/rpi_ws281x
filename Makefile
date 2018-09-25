

OBJ =  main.c dma.c mailbox.c pcm.c rpihw.c ws2811.c pwm.c

HEADER = clk.h dma.h gpio.h mailbox.h pcm.h pwm.h rpihw.h version.h ws2811.h
test: $(OBJ) $(HEADER)
	gcc -o test $(OBJ)

clean:
	rm -f test
