

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
#include <math.h>
#include <string.h>

#include <curses.h>

#define LIFETIME_MAX 16
#define GRAVITY_MAX 3

//Initial ball velocity
#define EASY_BALL_VX .2
#define EASY_BALL_VY .1
//Amount ball velocity increases per hit
#define EASY_BALL_V_INC .05


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
    if (lines_intersect(ball->px, ball->py, ball->x, ball->y, paddle->x, 
                        paddle->y + paddle->width, paddle->x, paddle->y)){
        return 1;
    }
    return 0;
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
    mvwaddch(win, (int)ball->py, (int)ball->px, ' ');
}

void draw_ball(WINDOW* win, struct Ball* ball, int color)
{
    mvwaddch(win, (int)ball->y, (int)ball->x, 'O' | COLOR_PAIR(color));
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
    struct Ball* ball = make_ball(game->width/2, game->height/2, EASY_BALL_VX, EASY_BALL_VY);
    struct Paddle* paddle = make_paddle(0, game->height/2, 5, 2);

    int tick_time = 10;
    
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
                if ((paddle->y + paddle->vel + paddle->width)  > game->height){
                    move_paddle_xy(paddle, paddle->x, 0);
                }else { move_paddle_dir(paddle, DOWN);
                }
                break;
        }

        /*int tmp = ball_intersect_paddle(ball, paddle);
        if (tmp){
           move_ball_xy(ball, paddle->x+1, paddle->y);
           ball->vx = abs(ball->vx);
        }else if ((!tmp) && (ball->x < 0)){
           move_ball_xy(ball, game->width/2, game->height/2);
        }*/

        if ((ball->x < paddle->x) && ((ball->y <= (paddle->y + paddle->width)) &&
              (ball->y >= paddle->y))){
            move_ball_xy(ball, paddle->x+1, ball->y);
            ball->vx = -ball->vx;
            //Calculate how the velocity should be affected...
            // Do this by calculating the distance between the center of the paddle and
            //where the ball has hit!
            float paddle_center = (float) paddle->y + ((float) paddle->width / 2.0);
            float dist = abs(paddle_center - ball->y);
            float prcnt = dist / ((float) paddle->width / 2.0);
            float vxi = (1 - prcnt) * EASY_BALL_V_INC;
            float vyi = prcnt * EASY_BALL_V_INC;
            
            ball->vx = sign(ball->vx) * (fabsf(ball->vx) + vxi);
            ball->vy = sign(ball->vy) * (fabsf(ball->vy) + vyi);

            //Increase score...
        } else if (ball->x < paddle->x){
            ball->vx = EASY_BALL_VX;
            ball->vy = EASY_BALL_VY;
            move_ball_xy(ball, game->width/2, game->height/2);
        }
        
        //We need to erase it now because???
        erase_ball(win, ball);

        if ((ball->x > game->width) || (ball->x < 0)){ball->vx = -ball->vx;}
        if ((ball->y > game->height) || (ball->y < 0)){ball->vy = -ball->vy;}

        move_ball(ball);
        
        erase_draw_ball(win, ball, RED);
        erase_draw_paddle(win, paddle, BLUE);
        
        usleep(tick_time*1000);
        refresh();
    }
    
    die();
    return 0;
}
