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

#ifndef INPUT_H
#define INPUT_H

typedef enum InputMode_e InputMode_t;
enum InputMode_e
{
	CMD_MODE = 0,
	INSERT_MODE,
	MODE_COUNT
};

typedef struct InputLib_s InputLib_t;
struct InputLib_s
{
	int (*on_key_down_cb)(void*, int c);
	int (*on_key_wait_cb)(void*);
};

typedef struct InputModeInfo_s InputModeInfo_t;
struct InputModeInfo_s
{
	InputMode_t mode;
	InputLib_t *lib;

	InputModeInfo_t *next, *prev;
};

void input_init();
InputModeInfo_t *input_mode_new(InputMode_t mode);
int input_register_lib(InputMode_t mode, InputLib_t* il);
int input_key_down(void *uptr, int c);
int input_key_wait(void *uptr);

#endif

