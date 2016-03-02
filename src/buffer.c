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
	b->c_line = 0;
	b->linecount = 0;
	b->linelns = NULL;
	b->fp = NULL;

	lprintf(LL_DEBUG, "Created a new buffer: %s", b->filename);

	bc = buf_chunk_new(b);
	buf_chunk_add(b, bc);

	b->tot_sz = BCHUNK_SZ;
	b->tot_len = 0;
	
	return b;
}

BChunk_t *buf_chunk_new(Buffer_t *b)
{
	BChunk_t *bc;

	bc = malloc(sizeof(BChunk_t));

	memset(bc->buf, '\0', sizeof(bc->buf));
	bc->next = 0;
	bc->prev = 0;

	lprintf(LL_DEBUG, "Created a new chunk (%zi bytes) to buffer %s", sizeof(bc->buf), b->filename);

	return bc;
}

char *buf_get_content(Buffer_t *b)
{
	BChunk_t *bc;
	int	  copied;
	char	 *s;

	copied = 0;
	s = malloc(b->tot_sz);

	bc = b->chk_start;
	while (bc)
	{
		snprintf(s + copied, (b->tot_sz - copied), "%s", bc->buf);

		copied += strlen(bc->buf);
		bc = bc->next;
	}

	s[copied] = '\0';

	return s;
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

	lprintf(LL_DEBUG, "Added a new chunk to buffer with filename %s", b->filename);

	b->tot_sz += BCHUNK_SZ;

	return 0;
}

void buf_chunk_free(Buffer_t* b, BChunk_t *bc)
{
	if (!bc)
		return;

	if (bc->prev)
		bc->prev->next = bc->next;
	else
		b->chk_start = bc->next;

	if (bc->next)
		bc->next->prev = bc->prev;

	lprintf(LL_DEBUG, "Chunk freed from the buffer with filename %s", b->filename);

	free(bc);
}

void buf_free(Buffer_t *b)
{
	BChunk_t *bc;

	bc = b->chk_start;
	while (bc)
	{
		buf_chunk_free(b, bc);

		bc = b->chk_start;
	}

	if (b->fp)
		fclose(b->fp);

	lprintf(LL_DEBUG, "Closed the buffer with filename %s", b->filename);

	if (b->linelns != NULL)
		free(b->linelns);

	free(b);
}

int buf_load_file(Buffer_t *b, const char *filename)
{
	int	   r, i, j, read_b, len, newline;
	char	   c;
	BChunk_t  *bc;

	if (b->fp)
		fclose(b->fp);

	b->fp = fopen(filename, "r");
	if (!b->fp)
	{
		lprintf(LL_ERROR, "Failed opening file %s|", filename);

		return -1;
	}

	lprintf(LL_DEBUG, "Opened file %s to a buffer", filename);
	read_b = 0;

	bc = b->chk_start;
	while (1)
	{
		r = read(fileno(b->fp), bc->buf, (sizeof(bc->buf) - 1));
		if (r < 0)
		{
			lprintf(LL_ERROR, "Failed to read file: %s", filename);

			return r;

		}
		else if (r == (sizeof(bc->buf) - 1))
		{
			bc = buf_chunk_new(b);
			buf_chunk_add(b, bc);
		}
		else if(r == 0)
			break;
		
		read_b += r;
	}

	bc = b->chk_start;
	while (bc)
	{
		i = 0;
		c = bc->buf[i];
		while (c)
		{
			if (c == '\n' || c == '\r')
				b->linecount++;

			if (c == '\0')
				break;

			c = bc->buf[++i];

			b->tot_len++;
		}

		bc = bc->next;
	}

	if (b->linelns != NULL)
	{
		lprintf(LL_CRITICAL, "Buffer %s not cleared before reuse!", b->filename);
		return -1;
	}

	b->linelns = malloc(b->linecount * sizeof(int));
	b->l_info =  malloc(b->linecount * sizeof(LineInfo_t));

	newline = 1;
	len = 0;
	j = 0;

	bc = b->chk_start;
	while (bc)
	{
		i = 0;
		c = bc->buf[i];
		while (c)
		{
			if (newline == 1)
				b->l_info[j].p = &bc->buf[i];

			if (c == '\n' || c == '\r')
			{
				b->l_info[j].n = len;
				b->linelns[j] = len;
				len = 0;
				newline = 1;

				j++;
			}

			if (c == '\0')
				break;

			c = bc->buf[++i];
			len++;

			newline = 0;
		}

		bc = bc->next;
	}

	lprintf(LL_DEBUG, "Buffer line count: %d", b->linecount);
	lprintf(LL_DEBUG, "%d bytes read from the file: %s", read_b, filename);

	return read_b;
}

int buf_save_file(Buffer_t *b, const char *filename)
{
	int	   r, written;
	BChunk_t  *bc;

	if (b->fp)
		fclose(b->fp);

	b->fp = fopen(filename, "w");
	if (!b->fp)
	{
		lprintf(LL_ERROR, "Failed opening file %s", filename);

		return -1;
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
			lprintf(LL_ERROR, "Failed to write to file: %s", b->filename);
			break;
		}

		if (r == 0)
		{
			break;
		}

		written += r;
		bc = bc->next;
	}

	lprintf(LL_ERROR, "%d bytes written to the file: %s", written, b->filename);

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
		lprintf(LL_DEBUG, "Buffer not big enough, allocating a new chunk", b->tot_sz);

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

int buf_clear(Buffer_t *b)
{
	BChunk_t *bc;

	bc = b->chk_start->next;

	while (bc)
	{
		buf_chunk_free(b, bc);
		bc = b->chk_start->next;
	}

	memset(b->chk_start->buf, 0, sizeof(b->chk_start->buf));

	b->c_pos = 0;
	b->linecount = 0;
	free(b->linelns);
	b->linelns = NULL;
	b->tot_sz = BCHUNK_SZ;

	return 0;
}

