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

#include "logic.h"
#include "draw.h"
#include "init.h"
#include "sound.h"
#include "fancymenu.h"




int game(int b_width, int b_height)
{
    //Create the ball & padlle

    struct Ball ball = {.y=2, .x=2, .px=0, .py=0, .vx=2, .vy=1};
    struct Ball* pball = &ball;

    struct Paddle paddle = {.y=12, .x=0, .px=0, .py=0, .width=7, .vel=2};
    struct Paddle* ppaddle = &paddle;
    
    
    
    //Screen size
    int boundy;
    int boundx;
    WINDOW *board = make_newwin(b_height, b_width, 0, 0, COLOR_GREEN);
    getmaxyx(board, boundy, boundx);
    boundx -= 1; //So stuff does not touch the border...
    boundy -= 1;
    
    //Create the windows
    
    WINDOW *status = make_newwin(b_height, boundx-b_height, 0, b_width+1, COLOR_RED);
    box(status, 0, 0);


    //Main loop
    int key;
    int lives = 2;
    int score = 0;
    int coll;
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
        if (check_ball(pball, boundx, boundy)) fbeep(440, 20);
        move_ball(pball);
        coll = collision(pball, ppaddle);//Check whether the ball has hit
        if (coll) fbeep(660, 20);
        switch (coll){
            case 1:
                ball.vx = -ball.vx;
                ball.x = paddle.x+1;
                break;
            case -1:
                ball.vx = abs(ball.vx);
                ball.x = boundx/2;
                ball.y = boundy/2;
                lives--;
                break;
            case 0:
                break;
            }
    //VIEW:
        if ((ball.y > paddle.y) && (ball.y < paddle.y + paddle.width)){
            draw_ball(board, pball, GREEN);
            draw_paddle(board, ppaddle, GREEN);
        }else {
            draw_ball(board, pball, RED);
            draw_paddle(board, ppaddle, RED);}
        mvwprintw(status, 2, 1, "Lives:%d", lives);
        mvwprintw(status, 3, 1, "Score:%d", score);
        wrefresh(board);
        wrefresh(status);
        //usleep takes microseconds which is 1/1000th of a millisecond
        //TODO: Figure out a good value for it...
        usleep(100000);
        
    }
    return 1;
}

    
int main(int argc,char **argv)
{
    initialize();
    char* items[2] = {"New Game", "Quit"};
    struct Menu* menu = new_menu(6, 2, 2, items, BLUE_ON_BLACK, GREEN_ON_BLACK);
    int x;
    while (1){
        x = poll_menu(menu);
        switch (x){
            case 0:
                game(70,24);
                break;
            case 1:
                endwin();
                exit(0);
                break;
            }
        }
    
    /*Make the game menu 'ere*/
    return die();
}
