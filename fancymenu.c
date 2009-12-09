//      fancymenu.c
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

#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

//#include "init.h"


struct Menu {
    WINDOW* win;
    int selected;
    int animcnt;
    int ITEM_COLOR;
    int SEL_ITEM_COLOR;
    int num_items;
    char** items;
};


struct Menu* new_menu(int startx, int starty, int num_items, char** items, int ITEM_COLOR, int SEL_ITEM_COLOR)
{
    int height, width;
    height = width = 50;
    
    struct Menu* menu = (struct Menu*) malloc(sizeof(struct Menu));
    
    menu->win = newwin(height, width, starty, startx);
    
    menu->num_items = num_items;
    menu->items = items;
    menu->animcnt = 0;
    menu->selected = 0;
    menu->ITEM_COLOR = ITEM_COLOR;
    menu->SEL_ITEM_COLOR = SEL_ITEM_COLOR;
    return menu;
}


void draw_menu(struct Menu* menu)
{
    wclear(menu->win);
    int i;
    wattron(menu->win, COLOR_PAIR(menu->ITEM_COLOR));
    for (i=0; i<menu->num_items; i++){
        mvwaddstr(menu->win, i, 4, menu->items[i]);
    }
    wattroff(menu->win, COLOR_PAIR(menu->ITEM_COLOR));
    wattron(menu->win, COLOR_PAIR(menu->SEL_ITEM_COLOR));
    mvwaddstr(menu->win, menu->selected, 4, menu->items[menu->selected]);
    wattroff(menu->win, COLOR_PAIR(menu->SEL_ITEM_COLOR));
    wattron(menu->win, COLOR_PAIR(menu->ITEM_COLOR));
    mvwaddstr(menu->win, menu->selected, 0, ">>>");
    wattroff(menu->win, COLOR_PAIR(menu->ITEM_COLOR));
    mvwaddch(menu->win, menu->selected, menu->animcnt, '>' | COLOR_PAIR(menu->SEL_ITEM_COLOR));
    if (++menu->animcnt > 2) menu->animcnt = 0;
    refresh(); //TODO: look into this refresh issues...
    wrefresh(menu->win);
}

int poll_menu(struct Menu* menu)
/*Show the menu and animation and return the index of the item chosen.*/
{
    /*TODO: Check for cbreak keypad &c.*/
    int ch;
    while ((ch = getch())){
        draw_menu(menu);
        
        switch (ch){
            case KEY_UP:
                if (--menu->selected < 0) menu->selected = menu->num_items-1;
                break;
            case KEY_DOWN:
                if (++menu->selected > menu->num_items-1) menu->selected = 0;
                break;
            case KEY_RIGHT:
                return menu->selected;
                break;
            }

        usleep(125*1000);
    }
    return 0;
}

/*
 *  int main()
{
    initscr();
    //We want color
    start_color();
    int BLUE_ON_BLACK = 1;
    int GREEN_ON_BLACK = 2;
    init_pair(BLUE_ON_BLACK, COLOR_BLUE, COLOR_BLACK);
    init_pair(GREEN_ON_BLACK, COLOR_GREEN, COLOR_BLACK);
    nodelay(stdscr, 1);
    char* items[3] = {"New Game", "End Game", "Quit"};
    struct Menu* menu = new_menu(3, items, BLUE_ON_BLACK, GREEN_ON_BLACK);
    int x;
    x = poll_menu(menu);
    //printf("%d\n", menu->num_items);
    //printf("%s\n%s\n%s\n", menu->items[0], menu->items[1], menu->items[2]);
    endwin();
    printf("%d\n", x);
    return 0;
}
*/
