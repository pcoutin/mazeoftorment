CC=		cc
CFLAGS=		-Os -c
LDFLAGS=	-L/usr/local/lib/ -lSDL2-2.0 -lSDL2_image

OBJFILES=	ini.o main.o cfg.o

all:	clc

ini:
	make -C inih all

main:
	${CC} ${CFLAGS} main.c

cfg:
	${CC} ${CFLAGS} cfg.c

clc:	ini main cfg
	${CC} ${LDFLAGS} -o clc ${OBJFILES}

clean:
	rm *.o clc
