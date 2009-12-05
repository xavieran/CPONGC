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
#include "logic.h"


int draw_paddle(WINDOW* win, struct Paddle* paddle, int color)
/*Draw the paddle in color*/
{
    //Erase the paddle
    int top = paddle->py + paddle->width;
    int y = paddle->py;
    for (; y < top; y++){
        mvwaddch(win, y, paddle->px, ' ');
    }
    //Now draw it again
    top = paddle->y + paddle->width;
    y = paddle->y;
    wattron(win, COLOR_PAIR(color));
    for (; y < top; y++){
        mvwaddch(win, y, paddle->x, ' ');
    }
    wattroff(win, COLOR_PAIR(color));
    return 0;
}

int draw_ball(WINDOW* win, struct Ball* ball, int color)
{
    mvwaddch(win, ball->py, ball->px, ' '); //erase the previous image
    mvwaddch(win, ball->y, ball->x, 'O' | COLOR_PAIR(color));
    return 0;
}

    
int draw_color(WINDOW* win, int y, int x, char txt, int color)
{
    mvwaddch(win, y, x, txt | COLOR_PAIR(color));
    return 0;
}

WINDOW* make_newwin(int height, int width, int starty, int startx, int color)
{
    WINDOW* win = newwin(height, width, starty, startx);
    return win;
}
