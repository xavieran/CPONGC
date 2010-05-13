

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

//#include "sound.h"
//#include "fancymenu.h"

enum directions {
   UP,
   DOWN,
   LEFT,
   RIGHT
};

enum SPECIALS {
   P_LONG
};

enum colors {
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        BLUE_ON_BLACK,
        RED_ON_BLACK,
        YELLOW_ON_BLACK,
        GREEN_ON_BLACK
        };


void die_no_memory(){
    printf("No more memory to allocate!\n");
    exit(1);
}


/*BALL STRUCTURE*/
struct Ball {
    int x; int y; //x, y coordinates
    int px; int py; //the previous position of the ball, used when drawing it
    int vx; int vy; //the x and y velocity of the ball
};

struct Ball* make_ball(int x, int y, int vx, int vy)
{
    struct Ball* p = malloc(sizeof(struct Ball));
    if (p == NULL) die_no_memory();
    p->x = x; p->y = y;
    p->px = x; p->py = y;
    p->vx = vx; p->vy = vy;
    return p;
}

void destroy_ball(struct Ball* ball)
{
    free(ball);
    ball = NULL;
}                  

char* str_ball(struct Ball* ball)
{
   char* msg = malloc(sizeof(char)*256);
   sprintf(msg, "(x,y):(%d,%d)   (px,py):(%d,%d)  (vx, vy):(%d,%d)",
         ball->x, ball->y, ball->px, ball->py, ball->vx, ball->vy);
   return msg;
}

/*PADDLE STRUCTURE*/
struct Paddle {
/* The paddle looks like this:
 * 0,0
 * | x,y-width (width=5)
 * |
 * |
 * |
 * | x,y
 * 0,y
 * 
 */   
    int x; int y; //x,y coordinates
    int px; int py; //the previous position of paddle
    int width; //the width
    int vel; //the amount it moves
};

struct Paddle* make_paddle(int x, int y, int width, int vel)
{
    struct Paddle* p = malloc(sizeof(struct Paddle));
    if (p == NULL) die_no_memory();
    p->x = x; p->y = y;
    p->px = x; p->py = y;
    p->width = width;
    p->vel = vel;
    return p;
}

void destroy_paddle(struct Paddle* paddle)
{             
    free(paddle);
    paddle = NULL;
}

char* str_paddle(struct Paddle* paddle)
{
   char* msg = malloc(sizeof(char)*256);
   sprintf(msg, "(x,y):(%d,%d)   (px,py):(%d,%d)   width:%d   vel:%d",
         paddle->x, paddle->y, paddle->px, paddle->py, paddle->width,
         paddle->vel);
   return msg;
}


struct Game {
   int lives;
   int score;
   int width;
   int height;
};


struct Game* make_game(int lives, int score, int width, int height)
{
    struct Game* p = malloc(sizeof(struct Game));
    if (p == NULL) die_no_memory();
    p->lives = lives; 
    p->score = score;
    p->width = width;
    p->height = height;
    return p;
}

void destroy_game(struct Game* game)
{             
    free(game);
    game = NULL;
}

char* str_game(struct Game* game)
{
   char* msg = malloc(sizeof(char)*256);
   sprintf(msg, "lives:%d  score:%d  (w, h): (%d, %d)",
         game->lives, game->score, game->width, game->height);
   return msg;

}

/*Logic*/
/*If a function is suffixed with f, it returns the changes it would make.
 *FIXME!!!!*/

struct Ball move_ball_f(struct Ball ball)
{
    ball.px = ball.x;
    ball.py = ball.y;
    ball.x += ball.vx;
    ball.y += ball.vy;
    return ball;
}

void move_ball(struct Ball* ball)
{   
    *ball = move_ball_f(*ball);
}


struct Paddle move_paddle_dir_f(struct Paddle paddle, int dir)
//Move paddle in the specified direction
{   
    switch (dir){
        case UP:
            paddle.py = paddle.y;
            paddle.y -= paddle.vel;
            break;
        case DOWN:
            paddle.py = paddle.y;
            paddle.y += paddle.vel;
            break;
    }
    return paddle;
}

void move_paddle_dir(struct Paddle* paddle, int dir)
//Move paddle in the specified direction
{
    *paddle = move_paddle_dir_f(*paddle, dir);
}


struct Paddle move_paddle_xy_f(struct Paddle paddle, int x, int y)
{
    paddle.px = paddle.x;
    paddle.py = paddle.y;
    paddle.x = x;
    paddle.y = y;
    return paddle;
}


void move_paddle_xy(struct Paddle* paddle, int x, int y)
{
    *paddle = move_paddle_xy_f(*paddle, x, y);
}

int lines_intersect(float x1, float y1, float x2, float y2, 
                    float a1, float b1, float a2, float b2)
{
    return 1
}
// Drawing Functions: 
void initialize_colors()
{
    //We want color
    start_color();
    init_pair(RED, COLOR_RED, COLOR_RED);
    init_pair(GREEN, COLOR_GREEN, COLOR_GREEN);
    init_pair(YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(CYAN, COLOR_CYAN, COLOR_CYAN);
    init_pair(WHITE, COLOR_WHITE, COLOR_WHITE);
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
    init_pair(RED_ON_BLACK, COLOR_RED, COLOR_BLACK);
    init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK);
    init_pair(GREEN_ON_BLACK, COLOR_GREEN, COLOR_BLACK);  
}

int die()
/*End curses.*/
{
    endwin();
    return 0;
}

void erase_rect(WINDOW* win, int y, int x, int length, int height)
{
    int i;
    int j;
    for (i=y; i<=(y+height); i++){
        for (j=x; j<(x+length); j++){
            mvwaddch(win, i, j, ' ');
        }
    }
}

void draw_ball(WINDOW* win, struct Ball* ball, int color)
{
    mvwaddch(win, ball->py, ball->px, ' '); //erase the previous image
    mvwaddch(win, ball->y, ball->x, 'O' | COLOR_PAIR(color));
}

void erase_paddle(WINDOW* win, struct Paddle* paddle)
{   
    erase_rect(win, paddle->py, paddle->px, 1, paddle->width);
}

void draw_paddle(WINDOW* win, struct Paddle* paddle, int color)
{
    int y;
    int bottom = paddle->y + paddle->width;
    wattron(win, COLOR_PAIR(color));
    for (y = paddle->y; y <= bottom; y++){
        mvwaddch(win, y, paddle->x, '|');
    }
    wattroff(win, COLOR_PAIR(color));
}

void erase_draw_paddle(WINDOW* win, struct Paddle* paddle, int color)
{
    //Erase the paddle
    erase_paddle(win, paddle);
    //Now draw it again
    draw_paddle(win, paddle, color);
}                     


int main(int argc, char** argv)
{
    WINDOW* win = malloc(sizeof(WINDOW));
    win = initscr();
    initialize_colors();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0); //turn cursor off
    nodelay(stdscr, 1);

    struct Game* game = make_game(2, 0, 80, 24);
    struct Ball* ball = make_ball(10, 10, 2, 1);
    struct Paddle* paddle = make_paddle(1, 6, 2, 2);
    
    int key;
    
    struct Paddle check_pad;
    while ((key = getch()) != KEY_RIGHT){
        switch (key) {
            case KEY_UP:
                //if ((paddle->y - paddle->vel) < 0){
                check_pad = move_paddle_dir_f(*paddle, UP);
                if (check_pad.y < 0){
                    move_paddle_xy(paddle, paddle->x, game->height-paddle->width);
                }else { move_paddle_dir(paddle, UP);
                }
                break;
            case KEY_DOWN:
                if ((paddle->y + paddle->vel)  > game->height + paddle->width){
                    move_paddle_xy(paddle, paddle->x, 0);
                }else { move_paddle_dir(paddle, DOWN);
                }
                break;
        }


        draw_ball(win, ball, RED);
        move_ball(ball);

        if ((ball->x > game->width) || (ball->x < 0)){ball->vx = -ball->vx;}
        if ((ball->y > game->height) || (ball->y < 0)){ball->vy = -ball->vy;}

        erase_draw_paddle(win, paddle, BLUE);
        usleep(50*1000);
        refresh();
    }
    die();
    return 0;
}
