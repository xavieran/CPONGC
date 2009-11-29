//      pong.c
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

int die()
{
    endwin();
    printf("%s","FINISHED\n");
    exit(0);
    return 0;
}



int main()
{
    //Create the ball & padlle

    struct Ball ball = {.y=2, .x=2, .vx=3, .vy=1};
    struct Ball *pball = &ball;

    struct Paddle paddle = {.y=12, .x=2, .width=2, .vel=2};
    struct Paddle *ppaddle = &paddle;
    
    /////////////
    //CURSES

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0); //turn cursor off
    nodelay(stdscr, 1);
    
    //We want color
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_RED);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_GREEN);
    int RED_ON_WHITE = 3;
    init_pair(RED_ON_WHITE, COLOR_RED, COLOR_WHITE);
    
    //Screen size
    int boundy;
    int boundx;
    getmaxyx(stdscr, boundy, boundx);

    //Main loop
    int key;
    int lives = 2;
    while (lives){
    //CONTROLLER:
        key = getch();
        switch (key){
            case KEY_UP:
                move_paddle(1, ppaddle, boundy);
                break;
            case KEY_DOWN:
                move_paddle(0, ppaddle, boundy);
                break;
            case ERR:
                break;
            }
    //MODEL
        check_ball(pball, boundx, boundy);
        move_ball(pball);
        //Check whether the ball has hit the bat or not...
        if ((ball.x+ball.vx < paddle.x) &&
            ((ball.y < paddle.y + paddle.width) && (ball.y > paddle.y - paddle.width))){
            ball.x = paddle.x+1;
            ball.vx = -ball.vx;
        } else if (ball.x <= 0){
            mvaddstr(0,0, "Lost a life");
            lives--;
        }
    //VIEW:
        clear();
        draw_paddle(ppaddle, COLOR_RED);
        draw_color(ball.y, ball.x, 'O', RED_ON_WHITE);
        mvprintw(0,0,"Lives:%d", lives);
        refresh();
        //usleep takes microseconds which is 1/1000th of a millisecond
        //TODO: Figure out a good value for it...
        usleep(50000);
        
    }
    /////////////
    
    return die();
}
