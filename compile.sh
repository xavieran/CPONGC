#!/bin/sh
INCLUDES="-lncurses"
INCLUDE_DIR="./"
EXE="game"

gcc -Wall -c *.c $INCLUDES -I$INCLUDE_DIR
gcc *.o $INCLUDES -I$INCLUDE_DIR -o $EXE