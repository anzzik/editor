/*
   This file is part of ctoolbox library collection. 
   Copyright (C) 2016 Anssi Kulju 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>

typedef struct Screen_s Screen_t;
struct Screen_s
{
	WINDOW *wd;
	int w;
	int h;
	int tl_x; /* tl == top-left */
	int tl_y;

	int c_x;  /* cursor */
	int c_y;
	int colors;
};

Screen_t *ncs_new();

int ncs_init(Screen_t *s);
void ncs_set_cursor(Screen_t *s, int x, int y);
void ncs_cursor_revert(Screen_t *s);
void ncs_start_color(Screen_t *s, int cp);
void ncs_stop_color(Screen_t *s, int cp);
void ncs_max_xy(Screen_t *s, int *x, int *y);
void ncs_cursor_lf(Screen_t *s, int n);
void ncs_cursor_rt(Screen_t *s, int n);
void ncs_cursor_up(Screen_t *s, int n);
void ncs_cursor_dw(Screen_t *s, int n);
void ncs_addch_xy(Screen_t *s, char c, int x, int y);
void ncs_addch(Screen_t *s, char c);
//void ncs_addstr(Screen_t *s, char *str, int x, int y, int n);
void ncs_addstr(Screen_t *s, int x, int y, const char *fmt, ...);
int ncs_render_data(Screen_t *s, char *p);
int  ncs_clear(Screen_t *s);
void ncs_close(Screen_t *s);
void ncs_quit();

#endif

