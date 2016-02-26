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

#include <stdlib.h>
#include <string.h>

#include "screen.h"

Screen_t *ncs_new()
{
	Screen_t *s = malloc(sizeof(Screen_t));

	return s;
}

int ncs_init(Screen_t *s)
{
	s->wd = initscr();
	if (s->wd == NULL)
	{
		return -1;
	}

	ncs_max_xy(s, &s->w, &s->h);

	s->colors = 0;
	if (has_colors())
	{
		start_color();

		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);

		s->colors = 1;
	}

	keypad(s->wd, TRUE);
	scrollok(s->wd, TRUE);
	nonl();
	cbreak();
	echo();

	wrefresh(s->wd);

	return 0;
}

void ncs_start_color(Screen_t *s, int cp)
{
	attron(COLOR_PAIR(cp));
}

void ncs_stop_color(Screen_t *s, int cp)
{
	attroff(COLOR_PAIR(cp));
}

void ncs_max_xy(Screen_t *s, int *x, int *y)
{
	getmaxyx(s->wd, *y, *x);
}

void ncs_set_cursor(Screen_t *s, int x, int y)
{
	wmove(s->wd, y, x);
	wrefresh(s->wd);

	s->c_x = x;
	s->c_y = y;
}

void ncs_cursor_revert(Screen_t *s)
{
	wmove(s->wd, s->c_y, s->c_x);
	wrefresh(s->wd);
}

void ncs_cursor_lf(Screen_t *s, int n)
{
	wmove(s->wd, s->c_y, s->c_x - n);
	s->c_x -=  n;

	wrefresh(s->wd);
}

void ncs_cursor_rt(Screen_t *s, int n)
{
	wmove(s->wd, s->c_y, s->c_x + n);
	s->c_x += n;

	wrefresh(s->wd);
}

void ncs_cursor_up(Screen_t *s, int n)
{
	wmove(s->wd, s->c_y - n, s->c_x);
	s->c_y -= n;

	wrefresh(s->wd);
}

void ncs_cursor_dw(Screen_t *s, int n)
{
	wmove(s->wd, s->c_y + n, s->c_x);
	s->c_y += n;

	wrefresh(s->wd);
}

void ncs_addch_xy(Screen_t *s, char c, int x, int y)
{
	mvwaddch(s->wd, y, x, c);
	wrefresh(s->wd);
}

void ncs_addch(Screen_t *s, char c)
{
	mvwaddch(s->wd, s->c_y, s->c_x, c);
	wrefresh(s->wd);
}

void ncs_addstr(Screen_t *s, int x, int y, int n, const char *fmt, ...)
{
	va_list ap;
	char	buffer[256];

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	mvwprintw(s->wd, y, x, "%s", buffer);

	wrefresh(s->wd);
}

int ncs_render_data(Screen_t *s, char *p)
{
	int  i;
	char c;

	for (i = 0; i < strlen(p); i++)
	{
		c = p[i];
		if (c == '\n')
		{
			ncs_set_cursor(s, 0, s->c_y + 1);

			continue;
		}

		if (c == '\t')
		{
			ncs_cursor_rt(s, 8);

			continue;
		}

		ncs_addch(s, c);
		ncs_cursor_rt(s, 1);
	}

	wrefresh(s->wd);

	return 0;
}

void ncs_close(Screen_t *s)
{
	wrefresh(s->wd);
	delwin(s->wd);

	free(s);
}

void ncs_quit()
{
	endwin();
}

