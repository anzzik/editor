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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

static InputModeInfo_t *modes[MODE_COUNT] = { NULL };
static InputMode_t current_mode = CMD_MODE;

void input_init()
{
	int i;

	for (i = 0; i < MODE_COUNT; i++)
	{
		modes[i] = input_mode_new(i);
	}
}

InputModeInfo_t *input_mode_new(InputMode_t mode)
{
	InputModeInfo_t *im;

	im = malloc(sizeof(InputModeInfo_t));
	im->next = 0;
	im->prev = 0;
	im->mode = mode;
	im->lib = malloc(sizeof(InputLib_t));

	return im;
}

int input_register_lib(InputMode_t mode, InputLib_t* il)
{
	if (!modes[mode])
	{
		fprintf(stderr, "lib register failed: mode is not initialized\n");

		return -1;
	}

	memcpy(modes[mode]->lib, il, sizeof(InputLib_t));

	return 0;
}

int input_key_down(void *uptr, int c)
{
	if (!modes[current_mode])
	{
		fprintf(stderr, "mode is not initialized, input hook missing\n");

		return -1;
	}

	modes[current_mode]->lib->on_key_down_cb(uptr, c);

	return 0;
}

int input_key_wait(void *uptr)
{
	int r;

	if (!modes[current_mode])
	{
		fprintf(stderr, "mode is not initialized, input hook missing\n");

		return -1;
	}

	r = modes[current_mode]->lib->on_key_wait_cb(uptr);

	return r;
}

void input_quit()
{
	int i;

	for (i = 0; i < MODE_COUNT; i++)
	{
		free(modes[i]->lib);
		free(modes[i]);
	}
}


