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

#ifndef EDITOR_H
#define EDITOR_H

#include "input.h"
#include "screen.h"
#include "buffer.h"

typedef struct Context_s Context_t;
struct Context_s
{
	InputMode_t  mode;
	Buffer_t    *c_buffer;
	Screen_t    *scr;
};

Context_t *ed_new();
int  ed_init(Context_t* ctx);
void ed_set_mode(Context_t *ctx, InputMode_t mode);
int  ed_loop(Context_t *ctx);
void ed_info(Context_t *ctx, const char *fmt, ...);
void ed_quit(Context_t *ctx);

#endif

