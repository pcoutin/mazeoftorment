CC=		cc
CFLAGS=		-Os -c
LDFLAGS=	-L/usr/local/lib/ -lSDL2-2.0 -lSDL2_image

OBJFILES=	ini.o main.o cfg.o maze_tiles.o

all:	clc

clc:	${OBJFILES}
	${CC} ${LDFLAGS} -o clc ${OBJFILES}

ini.o:
	make -C inih all

main.o: main.c
	${CC} ${CFLAGS} main.c

cfg.o: cfg.c
	${CC} ${CFLAGS} cfg.c

maze_tiles.o: entities/maze_tiles.c
	${CC} ${CFLAGS} entities/maze_tiles.c

clean:
	rm *.o clc
