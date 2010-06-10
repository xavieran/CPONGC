

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

//Satisfy GCC
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include <curses.h>

#include "sound.h"
#include "timer.h"
//#include "fancymenu.h"


#define MAX_STRING_LENGTH 256

#define LIFETIME_MAX 16
#define GRAVITY_MAX 3

//Initial ball velocity
#define EASY_BALL_VX .2
#define EASY_BALL_VY .075

//Amount ball velocity increases per hit
#define EASY_BALL_V_INC .05

#define EASY_AI_WAIT 20

#define INFO_WIN_HEIGHT 3



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

//Much thanks to http://members.cox.net/srice1/random/crandom.html
//Generate a random integer between 0 and h-1
int randint(int h)
{
    double r = rand();
    return (int) (r / (((double) RAND_MAX + (double) 1) / (double) h));
}
    

int sign(float x)
{
    if (x < 0) return -1;
    if (x == 0) return 0;
    return 1;
}

float abs_float(float x)
{
    if (x < 0) return -1.0 * x;
    return x;
}

int my_int(float x)
{
    if ((x - ((int) x)) > .5) return ((int) x) + 1;
    return (int) x;
}
    
/*++++++++++++++
 BALL STRUCTURE
 ++++++++++++++*/

struct Ball {
    float x; float y; //x, y coordinates
    float px; float py; //the previous position of the ball, used when drawing it
    float vx; float vy; //the x and y velocity of the ball
};

struct Ball* make_ball(float x, float y, float vx, float vy)
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
   sprintf(msg, "(x,y):(%f,%f)   (px,py):(%f,%f)  (vx, vy):(%f,%f)",
         ball->x, ball->y, ball->px, ball->py, ball->vx, ball->vy);
   return msg;
}

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


struct Ball move_ball_xy_f(struct Ball ball, float x, float y)
{
    ball.px = ball.x;
    ball.py = ball.y;
    ball.x = x;
    ball.y = y;
    return ball;
}


void move_ball_xy(struct Ball* ball, int x, int y)
{
    *ball = move_ball_xy_f(*ball, x, y);
}


/*++++++++++++++
 PADDLE STRUCTURE
 ++++++++++++++*/
struct Paddle {
/* The paddle looks like this:
 * 0,0
 * | x,y 
 * |
 * |
 * |
 * | x,y+width (width=5)
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
   sprintf(msg, "(x,y):(%d,%d) (px,py):(%d,%d) width:%d vel:%d",
         paddle->x, paddle->y, paddle->px, paddle->py, paddle->width,
         paddle->vel);
   return msg;
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


/*+++++++++++++++++++
 * PARTICLE STRUCTURE
 +++++++++++++++++++*/

struct Particle {
    int x; int y;
    int px; int py;
    int vx; int vy;
    int gravity;
    int life;
    int lifetime;
    int color;
};

struct Particle* make_particle(int x, int y, int vx, int vy, int color)
{
    struct Particle* p = malloc(sizeof(struct Particle));
    if (p == NULL) die_no_memory();
    p->x = x; p->y = y;
    p->px = x; p->py = y;
    p->vx = vx; p->vy = vy;
    p->gravity = randint(GRAVITY_MAX);
    p->life = 0;
    p->lifetime = randint(LIFETIME_MAX);
    p->color = color;
    return p;
}

void destroy_particle(struct Particle* particle)
{             
    free(particle);
    particle = NULL;
}

/*char* str_particle(struct Particle* particle)
{
   char* msg = malloc(sizeof(char)*256);
   sprintf(msg, "(x,y):(%d,%d)   (px,py):(%d,%d)   width:%d   vel:%d",
         particle->x, particle->y, particle->px, particle->py,
         particle->vel);
   return msg;
}*/

/*struct Particle move_particle_f(struct Particle particle)
{
    particle.px = particle.x;
    particle.py = particle.y;
    particle.x += particle.vx;
    particle.y += particle.vy;
    return particle;
}

void move_particle(struct Particle* particle)
{
    *particle = move_particle_f(*particle);
}

struct Particle** move_particles_array(struct Particle** particles)
{
    int length = sizeof(particles)/sizeof(struct Particle*);
    int i;
    struct Particle** live_particles = malloc(sizeof(struct Particle*) * length);
    int live_length = 0;

    for (i=0; i < length; i++){
        if !(particles[i]->life > particles[i]->lifetime){
            live_particles[live_length] = particles[i];
            live_length++;
        }
    }

    for (i=0; i < live_length; i++){
        live_particles[i] = move_particle(live_particles[i]);
    }
    return live_particles;
}*/


/*++++++++++++++
 GAME STRUCTURE
 ++++++++++++++*/
struct Game {
    int width;
    int height;
    int p1_score;
    int p2_score;
    char* p1_name;
    char* p2_name;
};


struct Game* make_game(int width, int height, char* p1_name, char* p2_name)
{
    struct Game* p = malloc(sizeof(struct Game));
    if (p == NULL) die_no_memory();
    p->width = width;
    p->height = height;
    p->p1_score = 0;
    p->p2_score = 0;
    p->p1_name = p1_name;
    p->p2_name = p2_name;
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
   sprintf(msg, "p1 score:%d  p2 score:%d  (w, h): (%d, %d)",
         game->p1_score, game->p2_score, game->width, game->height);
   return msg;

}

/*Logic*/
/*If a function is suffixed with f, it returns the changes it would make.
 *FIXME!!!!*/




//Shamelessly stolen from: http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
// intersect the intersection point may be stored in the floats i_x and i_y.
int lines_intersect(float p0_x, float p0_y, float p1_x, float p1_y, 
                    float p2_x, float p2_y, float p3_x, float p3_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        return 1;
    }

    return 0; // No collision
}

int ball_intersect_paddle(struct Ball* ball, struct Paddle* paddle)
{
    if (lines_intersect(ball->x, ball->y, ball->px, ball->py, paddle->x, 
                        paddle->y + paddle->width, paddle->x, paddle->y)){
        return 1;
    }
    return 0;
}

//Return 0 if ball within paddle, 1 if ball above paddle, -1 if ball below paddle
int ball_in_paddle(struct Ball* ball, struct Paddle* paddle)
{
    if (ball->y > (paddle->y + paddle->width)){
        return -1;
    } else if (ball->y < paddle->y){
        return 1;
    }

    return 0;
}

//Calculate the distance from the center of the paddle to where the ball has
//struck.
//Return the percentage such that p == 1 means that the ball has hit
//the very edge, and p == 0 means that the ball has hit the center
float collision_dist_prcnt(struct Ball* ball, struct Paddle* paddle)
{
    float paddle_center = (float) paddle->y + ((float) paddle->width / 2.0);
    float dist = abs(paddle_center - ball->y);
    float prcnt = dist / ((float) paddle->width / 2.0);
    return prcnt;
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



void erase_ball(WINDOW* win, struct Ball* ball)
{
    mvwaddch(win, my_int(ball->py), my_int(ball->px), ' ');
}

void draw_ball(WINDOW* win, struct Ball* ball, int color)
{
    mvwaddch(win, my_int(ball->y), my_int(ball->x), 'O' | COLOR_PAIR(color));
}

void erase_draw_ball(WINDOW* win, struct Ball* ball, int color)
{
    erase_ball(win, ball);
    draw_ball(win, ball, color);
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

void game(int difficulty, int ai);//...



int main(int argc, char** argv)
{
    //Eliminate compiler warning :)
    (void) argc;
    (void) argv;

    clock_t previous_t = clock() / (CLOCKS_PER_SEC / 1000);
    clock_t current_t;
    Timer* timer = sgl_timer_new();
    
    struct Game* game = make_game(80, 24 - INFO_WIN_HEIGHT + 1, "Computer", "Human");
    struct Ball* ball = make_ball(game->width/2, game->height/2, EASY_BALL_VX, EASY_BALL_VY);
    struct Paddle* paddle1 = make_paddle(0, game->height/2, 5, 3);
    struct Paddle* paddle2 = make_paddle(game->width-1, game->height/2, 5, 3);

    WINDOW* win = malloc(sizeof(WINDOW));
    initscr();
    
    win = newwin(game->height, game->width, INFO_WIN_HEIGHT, 0);
    
    WINDOW* info_win = newwin(INFO_WIN_HEIGHT, 80, 0, 0);
    
    initialize_colors();
    
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0); //turn cursor off
    nodelay(stdscr, 1);

    //int tick_time = 10;
    int ai_wait = 0;
    
    int key;
    struct Paddle check_pad;
    char* tmp_str = malloc(sizeof(char) * MAX_STRING_LENGTH);

    
    while ((key = getch())){// != KEY_RIGHT){

        switch (key) {
            case 'w':
                check_pad = move_paddle_dir_f(*paddle1, UP);
                if (check_pad.y + paddle1->vel < 0){
                    move_paddle_xy(paddle1, paddle1->x, game->height - paddle1->width);
                }else { move_paddle_dir(paddle1, UP);
                }
                break;
                
            case 's':
                check_pad = move_paddle_dir_f(*paddle1, UP);
                if ((paddle1->y + paddle1->vel + paddle1->width)  > game->height){
                    move_paddle_xy(paddle1, paddle1->x, 0);
                }else { move_paddle_dir(paddle1, DOWN);
                }
                break;
                
            case KEY_UP:
                check_pad = move_paddle_dir_f(*paddle2, UP);
                if ((check_pad.y + paddle2->vel) < 0){
                    move_paddle_xy(paddle2, paddle2->x, game->height - paddle2->width);
                }else { move_paddle_dir(paddle2, UP);
                }
                break;
                
            case KEY_DOWN:
                check_pad = move_paddle_dir_f(*paddle2, DOWN);
                if (((check_pad.y + check_pad.width) - paddle2->vel)  > game->height){
                    move_paddle_xy(paddle2, paddle2->x, 0);
                }else { move_paddle_dir(paddle2, DOWN);
                }
                break;
        }

        //AI!!!
        if (ai_wait > EASY_AI_WAIT && ball->vx < 0){
            switch (ball_in_paddle(ball, paddle1)) {
                case 0:
                    break;
                case 1:
                    move_paddle_dir(paddle1, UP);
                    break;
                case -1:
                    move_paddle_dir(paddle1, DOWN);
                    break;
            }
            ai_wait = 0;
        }
        
        //Ball has collided with paddle1

        
        //current_t = clock() / (CLOCKS_PER_SEC / 1000);
        if (sgl_timer_elapsed_milliseconds(timer) > 10){//((current_t - previous_t) > 1) {
            //previous_t = current_t;
            sgl_timer_reset(timer);
            ai_wait++;
            
            if ((ball->x <= paddle1->x) && ball_intersect_paddle(ball, paddle1)){
                fbeep(660, 10);
                //Move the ball 1 forward from paddle, this paddle is on left
                move_ball_xy(ball, paddle1->x + 1, ball->y);
                //Flip the ball's direction
                ball->vx = -ball->vx;
                
                float prcnt = collision_dist_prcnt(ball, paddle1);
                float vxi = (1 - prcnt) * EASY_BALL_V_INC;
                float vyi = prcnt * EASY_BALL_V_INC;
                
                ball->vx = sign(ball->vx) * (fabsf(ball->vx) + vxi);
                ball->vy = sign(ball->vy) * (fabsf(ball->vy) + vyi);
    
            } else if (ball->x <= paddle1->x){
                fbeep(550, 20);
                //Increase score...
                game->p2_score++;
                ball->vx = EASY_BALL_VX;
                ball->vy = EASY_BALL_VY;
                move_ball_xy(ball, game->width / 2, game->height / 2);
            }
            
            //Ball has collided with paddle2
            if ((ball->x >= paddle2->x) && ball_intersect_paddle(ball, paddle2)){
                fbeep(660, 10);
                //Move the ball 1 back from the paddle, this paddle is the one on right...
                move_ball_xy(ball, paddle2->x - 1, ball->y);
                ball->vx = -ball->vx;
                
                float prcnt = collision_dist_prcnt(ball, paddle2);
                float vxi = (1 - prcnt) * EASY_BALL_V_INC;
                float vyi = prcnt * EASY_BALL_V_INC;
                
                ball->vx = sign(ball->vx) * (fabsf(ball->vx) + vxi);
                ball->vy = sign(ball->vy) * (fabsf(ball->vy) + vyi);
    
            } else if (ball->x >= paddle2->x){
                //make bad noise :(
                fbeep(550, 20);
                //Increase score...
                game->p1_score++;
                ball->vx = -(EASY_BALL_VX);
                ball->vy = EASY_BALL_VY;
                move_ball_xy(ball, game->width / 2, game->height / 2);
            }
            
            //We need to erase it now because???
            erase_ball(win, ball);
    
            if ((ball->y > game->height) || (ball->y < 0)) ball->vy = -ball->vy;
            move_ball(ball);
        }
        
        
        erase_draw_ball(win, ball, RED);
        erase_draw_paddle(win, paddle1, BLUE);
        erase_draw_paddle(win, paddle2, GREEN);
        
        //Display some info
        box(info_win, 0 , 0);
        snprintf(tmp_str, MAX_STRING_LENGTH, "%s: %d", game->p1_name, game->p1_score);
        mvwaddstr(info_win, 1, 1, tmp_str);
        snprintf(tmp_str, MAX_STRING_LENGTH, "%s: %d", game->p2_name, game->p2_score);
        mvwaddstr(info_win, 1, 80 - strlen(tmp_str) - 1, tmp_str);
        
        wrefresh(info_win);
        wrefresh(win);
        refresh();
        
        //usleep(tick_time*1000);
    }
    
    die();
    return 0;
}
