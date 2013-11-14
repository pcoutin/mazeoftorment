#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include "mot_maze.h"

#define MOTSRV_ADDR     "10.0.0.2"
#define MOTSRV_PORT     "6666"
#define BACKLOG         8

#define MAZE_MAGIC      0x6D7A
#define ADD_PLAYER      0x4E45
#define HUNTER          0x4855
#define ILLEGAL_MOV     0xF000
#define PLAYER_MOV      0x4D4F
#define PLAYER_DC       0x4443
#define PLAYER_DIE      0x4B4F
#define PLAYER_WIN      0x5749
#define SRV_BUSY        0xEEEE

sendall(s, buf, len)
	char *buf;
	int len;
{
	int total = 0;
	int bytesleft = len;
	int n;

	while (total < len) {
		n = send(s, buf + total, bytesleft, 0);

		if (n == -1) {
			perror("sendall");
			exit(1);
		}
		total += n;
		bytesleft -= n;
	}
}

main(argc, argv)
	int argc;
	char *argv[];
{
	int ssockfd, csockfd, err, i;
	unsigned short  magic;
	unsigned int    u;
	int len, bytes_sent;
	char pname[32];
	unsigned char pnum;

	struct hostent *he;
	struct sockaddr *caddr;
	int addr_size;

	struct sockaddr_in my_addr;

	/*
	 * TODO: Put the server address and maze dimensions in a config file.
	 */
	genmaze(20, 20);

	ssockfd = socket(PF_INET, SOCK_STREAM, 0);

	if (ssockfd == -1) {
		perror("Failed to allocate socket descriptor.");
		return 1;
	}

	/*
	 * If system thinks the socket is on use but it isn't, fix it... 
	 */

	i = 1;
	if (setsockopt(ssockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof i)) {
		perror("setsockopt");
		return 1;
	}


	/*
	 * Bind socket to port. 
	 */

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(6666);
	my_addr.sin_addr.s_addr = inet_addr(MOTSRV_ADDR);
	memset(my_addr.sin_zero, 0, sizeof my_addr.sin_zero);

	if (bind(ssockfd, (struct sockaddr *) &my_addr, sizeof(my_addr))
	    == -1) {
		perror("Failed to bind to port");
		return 1;
	}

	if (listen(ssockfd, BACKLOG) == -1) {
		perror("Failed to listen to connections");
		return 1;
	}


	/*
	 * Accept connections, finally! 
	 */

	addr_size = sizeof(caddr);
	csockfd = accept(ssockfd, (struct sockaddr *) & caddr, &addr_size);

	if (csockfd == -1) {
		perror("Cannot accept connections");
		return 1;
	}

	/*
	 * When a player first connects, send maze magic, data width, size.
	 * Then send the maze itself. Then await confirmation. 
	 */

	magic = htons(MAZE_MAGIC);
	sendall(csockfd, (char *) &magic, sizeof(magic));

	u = htonl(MAZE.w);
	sendall(csockfd, (char *) &u, sizeof(u));

	u = htonl(MAZE.size);
	sendall(csockfd, (char *) &u, sizeof(u));

	sendall(csockfd, MAZE.data, MAZE.size);

	if (recv(csockfd, &magic, sizeof(magic), 0) != sizeof(magic) ||
	    ntohs(magic) != MAZE_MAGIC) {
		fprintf(stderr, "Failed to get client confirmation\n");
		exit(1);
	}


	/*
	 * Receive player name (32 byte string), then send player number. 
	 */

	recv(csockfd, pname, 32, 0);
	printf("%s connected!!!\n", pname);

	/* this is testing */
	pnum = 0;
	sendall(csockfd, &pnum, sizeof(pnum));

	/*
	 * Add a few players scattered across the maze, then pick one as the
	 * predator... 
	 */
	for (pnum = 0; pnum < 12; pnum++) {
		/* player no */
		magic = htons(ADD_PLAYER);
		sendall(csockfd, (char *) &magic, sizeof(magic));
		sendall(csockfd, &pnum, sizeof(pnum));

		/* x and y, there can be collisions but who cares?????? */
		magic = htons((random() % 19) * 2);
		sendall(csockfd, (char *) &magic, sizeof(magic));

		magic = htons((random() % 19) * 2);
		sendall(csockfd, (char *) &magic, sizeof(magic));

		sendall(csockfd, pname, 32);
	}

	magic = htons(HUNTER);
	sendall(csockfd, (char *) &magic, sizeof(magic));

	pnum = random() % 12;
	sendall(csockfd, &pnum, sizeof(pnum));

	/*
	 * Free things (sockets, addrinfo, player data, maze) and exit. 
	 */
	close(ssockfd);
	close(csockfd);
	free(MAZE.data);

	return 0;
}
