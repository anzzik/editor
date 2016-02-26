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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define lprintf( level, format, args... ) \
 log_f( __FILE__, __LINE__, level, format, ## args )

typedef enum LogLevel_e LogLevel_t;
enum LogLevel_e
{
	LL_DEBUG = 0,
	LL_NOTICE,
	LL_WARNING,
	LL_ERROR,
	LL_CRITICAL,
	LL_COUNT
};

typedef struct LogLevelConf_s LogLevelConf_t;
struct LogLevelConf_s
{
	LogLevel_t level;
	FILE *fp;

	LogLevelConf_t *next, *prev;
};

void log_f(const char* filename, int linenumber, LogLevel_t level, const char* fmt, ...);
int log_add(LogLevel_t l, char *filename);
void log_quit();

#endif

