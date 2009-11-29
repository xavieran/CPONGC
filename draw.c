//      draw.c: some useful drawing functions
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


#include <curses.h>
#include "misc.h"


void draw_paddle(struct Paddle * paddle, int color)
/*Draw the paddle in color*/
{
    int top = paddle->y - paddle->width;
    int bottom = paddle->y + paddle->width;
    attron(COLOR_PAIR(color));
    for (; top < bottom; top++){
        mvaddch(top, paddle->x, '|');
    }
    attroff(COLOR_PAIR(color));
}


int draw_color(int y, int x, char txt, int color)
{
    mvaddch(y, x, txt | COLOR_PAIR(color));
    return 0;
}
