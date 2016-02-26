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
#include <unistd.h>

#include "buffer.h"
#include "log.h"

Buffer_t *buf_new(char *filename)
{
	Buffer_t *b;
	BChunk_t *bc;

	b = malloc(sizeof(Buffer_t));
	b->filename = filename;
	b->c_pos = 0;
	b->fp = NULL;

	lprintf(LL_DEBUG, "Created a new buffer: %s", b->filename);

	bc = buf_chunk_new(b);
	buf_chunk_add(b, bc);

	b->tot_sz = BCHUNK_SZ;
	
	return b;
}

BChunk_t *buf_chunk_new(Buffer_t *b)
{
	BChunk_t *bc;

	bc = malloc(sizeof(BChunk_t));

	memset(bc->buf, '\0', sizeof(bc->buf));
	bc->next = 0;
	bc->prev = 0;

	lprintf(LL_DEBUG, "Created a new chunk to buffer %s", b->filename);

	return bc;
}

int buf_chunk_add(Buffer_t *b, BChunk_t *bc)
{
	BChunk_t *c;

	c = b->chk_start;
	if (!c)
	{
		b->chk_start = bc;
	}
	else
	{
		while (c)
		{
			if (!c->next)
			{
				c->next = bc;
				bc->prev = c;

				break;
			}

			c = c->next;
		}
	}

	lprintf(LL_DEBUG, "Added a new chunk (%d bytes) to buffer with filename %s", BCHUNK_SZ, b->filename);

	b->tot_sz += BCHUNK_SZ;

	return 0;
}

void buf_chunk_close(Buffer_t* b, BChunk_t *bc)
{
	if (!bc)
		return;

	if (bc->prev)
		bc->prev->next = bc->next;
	else
		b->chk_start = bc->next;

	if (bc->next)
		bc->next->prev = bc->prev;

	free(bc);
}

void buf_close(Buffer_t *b)
{
	BChunk_t *bc;

	bc = b->chk_start;
	while (bc)
	{
		buf_chunk_close(b, bc);

		bc = b->chk_start;
	}

	free(b);
}

int buf_open_file(Buffer_t *b, const char *filename)
{
	b->fp = fopen(filename, "w+");
	if (!b->fp)
	{
		return -1;
	}

	lprintf(LL_DEBUG, "Opened file %s", filename);

	return 0;
}

int buf_load_file(Buffer_t *b, const char *filename)
{
	int	   r, i;
	BChunk_t  *bc;

	if (b->fp == NULL)
	{
		buf_open_file(b, filename);
	}

	bc = b->chk_start;
	while (1)
	{
		r = read(fileno(b->fp), bc->buf, (sizeof(bc->buf) - 1));
		if (r < 0)
		{
			fprintf(stderr, "read() failed in file: %s\n", filename);

			return -1;
		}

		if (r == 0)
		{
			break;
		}

		if (r == (sizeof(bc->buf) - 1))
		{
			i += r;
			bc = buf_chunk_new(b);
			buf_chunk_add(b, bc);
		}
	}

	b->filename = filename;

	return 0;
}

int buf_save_file(Buffer_t *b, const char *filename)
{
	int	   r, written;
	BChunk_t  *bc;

	if (b->fp == NULL)
	{
		buf_open_file(b, filename);
	}

	written = 0;
	bc = b->chk_start;
	while (bc)
	{
		fseek(b->fp, written, SEEK_SET);
		fflush(b->fp);

		lprintf(LL_DEBUG, "Buffer to write: %s", bc->buf);

		r = write(fileno(b->fp), bc->buf, (strlen(bc->buf)));
		if (r < 0)
		{
			break;
		}

		if (r == 0)
		{
			break;
		}

		written += r;
		bc = bc->next;
	}

	return r;
}

int buf_set_cursor(Buffer_t *b, int n)
{
	b->c_pos = n;

	return 0;
}

int buf_add_ch(Buffer_t *b, char c)
{
	BChunk_t *bc;
	int n;
	int offset;
	int chunk_n;
	
	if (b->c_pos >= b->tot_sz)
	{
		lprintf(LL_DEBUG, "Buffer not big enough: %d bytes", b->tot_sz);
		bc = buf_chunk_new(b);
		buf_chunk_add(b, bc);
	}

	offset = b->c_pos;
	n = BCHUNK_SZ;
	chunk_n = 1;

	bc = b->chk_start;
	while (b->c_pos > (n - 1))
	{
		bc = bc->next;
		n += BCHUNK_SZ;
		offset -= BCHUNK_SZ;

		chunk_n++;
	}

	bc->buf[offset] = c;

	b->c_pos++;

	return 0;
}

