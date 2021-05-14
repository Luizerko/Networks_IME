CFLAGS = -g -Wall

all: ep1

ep1: mac0352-servidor-exemplo-ep1.o
	gcc mac0352-servidor-exemplo-ep1.o -o ep1

mac0352-servidor-exemplo-ep1.o: mac0352-servidor-exemplo-ep1.c
	gcc $(CFLAGS) mac0352-servidor-exemplo-ep1.c -c

clean:
	rm -rf *.o ep1 topico*
