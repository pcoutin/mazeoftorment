CC=		cc
CFLAGS=		-Os -c
LDFLAGS=	-lSDL2-2.0

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
