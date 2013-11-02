CC=		cc
CFLAGS=		-Wall -O2 -c -I/usr/local/include
LDFLAGS=	-L/usr/local/lib/ -lGL -lSDL2-2.0 -lSDL2_image

OBJFILES=	main.o cfg.o maze.o picture.o player.o ini.o

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

picture.o: entities/picture.c
	${CC} ${CFLAGS} entities/picture.c

clean:
	rm *.o mot
