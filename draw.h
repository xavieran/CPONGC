//      draw.h
//      
//      Copyright 2009 Emmanuel Jacyna <xavieran.lives@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

#ifndef DRAW_H
#define DRAW_H

#include "logic.h"

int draw_paddle(WINDOW* win, struct Paddle* paddle, int color);

int draw_ball(WINDOW* win, struct Ball* ball, int color);

int draw_color(WINDOW* win, int y, int x, char txt, int color);


WINDOW* make_newwin(int height, int width, int starty, int startx, int color);
#endif
