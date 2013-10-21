# Maze of Torment

The current idea for this project is to build a game involving a maze. And
several players. And UDP/IP sockets.

Maybe there will be Lua or Squirrel integration.

On *nix, it should build if you have SDL and SDL_image 2.0. The server
will not need anything special besides BSD sockets and the srand48 and
drand48 functions, which are present at least on Linux and FreeBSD.
