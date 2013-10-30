CC=		cc
CFLAGS=		-Wall -O0 -g -c -I/usr/local/include
LDFLAGS=	-L/usr/local/lib/ -lSDL2-2.0 -lSDL2_image

OBJFILES=	main.o cfg.o maze.o player.o ini.o

all:	mot

mot:	${OBJFILES}
	${CC} ${LDFLAGS} -o mot ${OBJFILES}

main.o: main.c
	${CC} ${CFLAGS} main.c

cfg.o: cfg.c
	${CC} ${CFLAGS} cfg.c

maze.o: entities/maze.c
	${CC} ${CFLAGS} entities/maze.c

ini.o:	../common/inih/ini.c
	cd ../common/inih; cc -Os -o ../../client/ini.o -c ini.c

player.o: entities/player.c
	${CC} ${CFLAGS} entities/player.c

clean:
	rm *.o mot
