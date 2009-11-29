//      misc.h
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
    int x; 
    int y;
    int vx;
    int vy;
};

struct Paddle {
    int x;
    int y;
    int width;
    int vel;
};

void check_ball(struct Ball * ball, int boundx, int boundy);

void move_ball(struct Ball * ball);

void move_paddle(int dir, struct Paddle * paddle, int maxy);
