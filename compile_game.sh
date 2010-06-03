#!/bin/sh
INCLUDES="-lncurses"
INCLUDEDIR="./"
OUT=pong

gcc --std=c99 -ggdb3 -Wall -Wextra -pedantic $INCLUDES -I$INCLUDEDIR $1 -o $OUT
