CC=		clang
CFLAGS=		-Wall -O2 -c
LDFLAGS=	-lGL -lSDL2 -lSDL2_image -lSDL2_net

OBJFILES=	main.o cfg.o maze.o picture.o player.o ini.o net.o

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

net.o: net.c
	${CC} ${CFLAGS} net.c

clean:
	rm *.o mot
