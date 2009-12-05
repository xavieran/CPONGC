#!/bin/sh
INCLUDES="-lncurses -lmenu"
INCLUDE_DIR="./"
EXE="game.x"

gcc -Wall -c *.c $INCLUDES -I$INCLUDE_DIR
gcc *.o $INCLUDES -I$INCLUDE_DIR -o $EXE