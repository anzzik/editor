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

#define LI_CHUNK_SZ 128 

Buffer_t *buf_new(char *filename)
{
	Buffer_t *b;
	BChunk_t *bc;
	int i = 0;

	b = malloc(sizeof(Buffer_t));
	b->filename = filename;
	b->c_pos = 0;
	b->c_line = 0;
	b->linecount = 0;

	b->l_info = calloc(LI_CHUNK_SZ, sizeof(LineInfo_t));

	for (i = 0; i < LI_CHUNK_SZ; i++)
	{
		b->l_info[i].p = NULL;
		b->l_info[i].n = 0;
	}

	b->li_count = 1;
	
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

int buf_add_li_chunks(Buffer_t *b, int n)
{
	lprintf(LL_DEBUG, "Starting to add chunks");
	LineInfo_t *li;

	li = realloc(b->l_info, (b->li_count + n) * LI_CHUNK_SZ * sizeof(LineInfo_t));

	if (li == NULL)
	{
		lprintf(LL_CRITICAL, "realloc failed to expand the mem area");
		return -1;
	}
	
	b->li_count += n;
	b->l_info = li;
	lprintf(LL_DEBUG, "%d chunks added successfully", n);

	return 0;
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

	if (b->l_info != NULL)
		free(b->l_info);

	free(b);
}

int buf_load_file(Buffer_t *b, const char *filename)
{
	int	   r, i, j, read_b, l_len, newline;
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
	l_len = 0;
	while (bc)
	{
		i = 0;
		c = bc->buf[i];
		if (i == 0 && c != '\0' && bc == b->chk_start)
			b->linecount++;

		while (c)
		{
			if (l_len == 0)
				b->l_info[b->linecount - 1].p = &bc->buf[i];

			l_len++;

			if (c == '\n' || c == '\r')
			{
				b->l_info[b->linecount - 1].n = l_len;
				l_len = 0;
				b->linecount++;
				
				if (b->linecount >= b->li_count * LI_CHUNK_SZ)
					buf_add_li_chunks(b, 1);
			}

			if (c == '\0')
				break;

			c = bc->buf[++i];
			b->tot_len++;
		}

		bc = bc->next;
	}

	buf_dump_lineinfo(b);


	lprintf(LL_DEBUG, "Buffer line count: %d", b->linecount);
	lprintf(LL_DEBUG, "%d bytes read from the file: %s", read_b, filename);

	return read_b;
}

void buf_dump_lineinfo(Buffer_t *b)
{
	LineInfo_t *l;
	int i;

	i = 0;

	for (i = 0; i < 128; i++)
	{
		l = &b->l_info[i];
		lprintf(LL_DEBUG, "l_len:%d", l->n);
	}

	while (l->n > 0)
	{
//		lprintf(LL_DEBUG, "l_len:%d", l->n);

		l = &b->l_info[++i];
	}
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
	b->tot_sz = BCHUNK_SZ;

	return 0;
}

