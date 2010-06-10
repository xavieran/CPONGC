#!/bin/sh
INCLUDES="-lncurses"
INCLUDE_DIR="./"
SOURCE_FILES="sound.c pong.c fancymenu.c timer.c"
EXE="pong"

gcc -Wall -c $SOURCE_FILES $INCLUDES -I$INCLUDE_DIR
gcc *.o $INCLUDES -I$INCLUDE_DIR -o $EXE
rm *.o
