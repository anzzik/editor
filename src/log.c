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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

static LogLevelConf_t *llc[LL_COUNT];

void log_f(const char* filename, int linenumber, LogLevel_t level, const char* fmt, ...)
{
	va_list ap;
	char	buffer[256];
	time_t  t;
	struct	tm *tinfo;
	char    ts[20];
	int	i;
	LogLevelConf_t *c;

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	t = time(NULL);
	tinfo = localtime(&t);
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tinfo);

	for (i = level; i >= 0; i--)
	{
		c = llc[i];
		while (c)
		{
			fprintf(c->fp, "%s %s:%d %s\n", ts, filename, linenumber, buffer);
			fflush(c->fp);
			c = c->prev;
		}
	}

}

int log_add(LogLevel_t l, char *filename)
{
	LogLevelConf_t *c;

	c = malloc(sizeof(LogLevelConf_t));;

	c->fp = fopen(filename, "a");
	if (c->fp == NULL)
	{
		return -1;
	}

	c->level = l;
	c->next = 0;
	c->prev = llc[l];

	if (llc[l])
		llc[l]->next = c;

	llc[l] = c;

	return 0;
}

void log_quit()
{
	int i;
	LogLevelConf_t *l, *tmp;

	for (i = 0; i < LL_COUNT; i++)
	{
		if (llc[i])
		{
			l = llc[i];
			while (l)
			{
				fclose(l->fp);

				tmp = l->next;
				free(l);
				l = tmp;
			}
		}
	}
}

