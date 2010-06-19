#Makefile for CPONGC
##

SHELL=/bin/sh
DEBUG_OPTIONS=-std=c99 -ggdb3 -Wall -Wextra -pedantic

SRCDIR=./src/
SOURCES_=sound.c pong.c fancymenu.c timer.c
SOURCES=$(addprefix $(SRCDIR),$(SOURCES_))

INCLUDES_=ncurses
INCLUDES=$(addprefix -l,$(INCLUDES_))
MY_INCLUDES=./src/includes/

EXE=pong


all:
	$(CC) $(INCLUDES) -I$(MY_INCLUDES) $(SOURCES) -o $(EXE)

debug:
	$(CC) $(DEBUG_OPTIONS) $(INCLUDES) -I$(MY_INCLUDES) $(SOURCES) -o $(EXE)

