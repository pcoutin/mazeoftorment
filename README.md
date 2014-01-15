# Maze of Torment

The current idea for this project is to build a game involving a maze. And
several players. And TCP/IP sockets, as well as LuaJIT + FFI integration,
maybe.

On *nix, the client should build if you have SDL, SDL_net and SDL_image
2.0. It might require LuaJIT later on. The server will probably not need
anything special besides POSIX sockets, probably libev or maybe select(),
and the srand48 and drand48 functions, which are present at least on
Linux, FreeBSD and OpenBSD.

todo:
* make the killed player receive the PLAYER_DEATH info
* ...
