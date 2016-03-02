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

#ifndef BUFFER_H
#define BUFFER_H

#define BCHUNK_SZ 128

typedef struct BChunk_s BChunk_t;
struct BChunk_s
{
	char	  buf[BCHUNK_SZ + 1];

	BChunk_t *next, *prev;
};

typedef struct Buffer_s Buffer_t;
struct Buffer_s
{
	FILE	   *fp;
	int	    c_pos;
	size_t      tot_sz;
	char	   *name;
	BChunk_t   *chk_start;
	char	   *filename;
};

Buffer_t *buf_new(char *filename);
BChunk_t *buf_chunk_new(Buffer_t *b);
int	  buf_chunk_add(Buffer_t *b, BChunk_t *bc);
void	  buf_chunk_free(Buffer_t* b, BChunk_t *bc);
char	 *buf_get_content(Buffer_t *b);
void	  buf_free(Buffer_t *b);
int	  buf_load_file(Buffer_t *b, const char *filename);
int	  buf_save_file(Buffer_t *b, const char *filename);
int	  buf_set_cursor(Buffer_t *b, int n);
int	  buf_add_ch(Buffer_t *b, char c);
int	  buf_clear(Buffer_t *b);

#endif

