CC=		cc
CFLAGS=		-Os -c
LDFLAGS=	-L/usr/local/lib/ -lSDL2-2.0 -lSDL2_image

OBJFILES=	ini.o main.o cfg.o

all:	clc

clc:	ini.o main.o cfg.o
	${CC} ${LDFLAGS} -o clc ${OBJFILES}

ini.o:
	make -C inih all

main.o: main.c
	${CC} ${CFLAGS} main.c

cfg.o: cfg.c
	${CC} ${CFLAGS} cfg.c

clean:
	rm *.o clc
