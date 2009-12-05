//      misc.c
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




struct Ball {
    int x; int y; //x, y coordinates
    int px; int py; //the previous position of the ball, used when drawing it
    int vx; int vy; //the x and y velocity of the ball
};

struct Paddle {
/* The paddle looks like this:
 * 
 * | x,y+width (width=5)
 * |
 * |
 * |
 * | x,y
 * 
 */
    int x; int y; //x,y coordinates
    int px; int py; //the previous position of paddle
    int width; //the width
    int vel; //the amount it moves
};

int check_ball(struct Ball* ball, int boundx, int boundy)
/*Check whether the ball is out of boundaries,
 * and if it is, it changes the velocity accordingly*/
{
    if ((ball->x > boundx) || (ball->x < 0)){
        ball->vx = -ball->vx;
        return 1;
    }
    if ((ball->y > boundy) || (ball->y < 0)){
        ball->vy = -ball->vy;
        return 1;
    }
    
    return 0;
}

void move_ball(struct Ball* ball)
/*Move the ball*/
{
    ball->px = ball->x;
    ball->py = ball->y;
    ball->x += ball->vx;
    ball->y += ball->vy;
}

void move_paddle(int dir, struct Paddle* paddle, int maxy)
/*Move the paddle in dir. If dir is true, move up, else move down.
 * This also wraps around the top and bottom of the screen.*/
{
    paddle->py = paddle->y;
    if (dir){
        paddle->y -= paddle->vel;
        if (paddle->y < 0){paddle->y = maxy;}//Wrap to the top
    } else {
        paddle->y += paddle->vel;
        if (paddle->y > maxy){paddle->y = 0;}//Wrap to the bottom
    }
}

int between(int x, int a, int b)
{
/*Return 1 if x is between a and b.
 * ie. a<x<b*/
    if ((a < x) && (x < b)){
        return 1;
    }
    return 0;
}

int collision(struct Ball* ball, struct Paddle* paddle)
/*Check for collision between ball and bat.
 * return 1 if there is a collision
 * return -1 if the ball has gone over the boundary
 * return 0 if there is no collision
 */
{
    if ((ball->x-1 <= paddle->x) &&
        (between(ball->y, paddle->y, paddle->y+paddle->width))){
        return 1;
    } else if (ball->x < 0){
        return -1;
    }
    return 0;
}


