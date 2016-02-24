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
#include "input_cmd.h"


int input_cmd_key_wait(void *uptr)
{
	int c;

	c = getchar();

	return c;
}

int input_cmd_key_down(void *uptr, int c)
{
	switch (c)
	{
		case 'f':
			break;

		case 'q':
			break;

		case 'h':
			break;

		case 'j':
			break;

		case 'l':
			break;

		case 'i':
			break;

		default:
			break;
	}

	return 0;
}

