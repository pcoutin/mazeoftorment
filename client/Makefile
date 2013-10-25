CC=		cc
CFLAGS=		-Wall -I/usr/include/lua5.1/ -Os -c
LDFLAGS=	-L/usr/local/lib/ -lSDL2-2.0 -lSDL2_image -llua5.1

OBJFILES=	main.o cfg.o maze.o

all:	mot

mot:	${OBJFILES}
	${CC} ${LDFLAGS} -o mot ${OBJFILES}

main.o: main.c
	${CC} ${CFLAGS} main.c

cfg.o: cfg.c
	${CC} ${CFLAGS} cfg.c

maze.o: entities/maze.c
	${CC} ${CFLAGS} entities/maze.c

clean:
	rm *.o mot
