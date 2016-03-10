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

#define LI_CHUNK_SZ 2 

Buffer_t *buf_new(char *filename)
{
	Buffer_t *b;

	b = malloc(sizeof(Buffer_t));
	b->filename = filename;
	b->c_pos = 0;
	b->c_line = 0;
	b->c_col = 0;
	b->eolcount = 0;
	b->chk_start = NULL;
	b->fp = NULL;
	b->tot_sz  = 0;
	b->tot_len = 0;

	b->l_info = calloc(LI_CHUNK_SZ, sizeof(LineInfo_t));
	b->li_count = LI_CHUNK_SZ;

	lprintf(LL_DEBUG, "Created a new buffer: %s", b->filename);

	return b;
}

BChunk_t *buf_chunk_new(Buffer_t *b, size_t size)
{
	BChunk_t *bc;

	bc = malloc(sizeof(BChunk_t));
	bc->buf = malloc(size + 1);

	memset(bc->buf, '\0', size + 1);
	bc->next = 0;
	bc->prev = 0;
	bc->size = size;

	lprintf(LL_DEBUG, "Created a new chunk (%zi bytes) to buffer %s", size, b->filename);

	return bc;
}

int buf_li_add(Buffer_t *b, int n)
{
	LineInfo_t *li;
	int i;

	li = realloc(b->l_info, (b->li_count + n) * sizeof(LineInfo_t));
	if (li == NULL)
	{
		lprintf(LL_CRITICAL, "realloc failed to expand the mem area");
		return -1;
	}

	if (li != b->l_info)
	{
		lprintf(LL_DEBUG, "realloc returned a different pointer (%p <> %p)", li, b->l_info);
	}

	b->l_info = li;

	for (i = b->li_count; i < b->li_count + n; i++)
	{
		b->l_info[i].n = 0;
		b->l_info[i].p = NULL;
	//	memset(&b->l_info[i], '\0', sizeof(LineInfo_t));
	}

	b->li_count += n;

	lprintf(LL_CRITICAL, "LineInfo allocation successful");

	return 0;
}

char buf_get_char(Buffer_t *b, int line, int col)
{
	char *p;

	if (line > b->eolcount)
		return '\0';

	if (line > b->li_count)
		return '\0';

	if (col > b->l_info[line].n)
		return '\0';


	p = b->l_info[line].p;

	return *(p + col);
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

	b->tot_sz += bc->size;

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

	free(bc->buf);
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
	int	   r, i, l_len, c_line;
	char	   c;
	size_t	   f_sz;
	BChunk_t  *bc;

	if (b->fp)
		fclose(b->fp);

	b->fp = fopen(filename, "r");
	if (!b->fp)
	{
		lprintf(LL_ERROR, "Failed opening file %s|", filename);

		return -1;
	}

	r = fseek(b->fp, 0, SEEK_END);
	if (r)
	{
		lprintf(LL_ERROR, "fseek failed: %s\n", filename);
		fclose(b->fp);

		return -1;
	}

	f_sz = ftell(b->fp);
	if (f_sz == -1L)
	{
		lprintf(LL_ERROR, "ftell failed: %s\n", filename);
		fclose(b->fp);

		return -1;
	}

	rewind(b->fp);
	fflush(b->fp);

	lprintf(LL_DEBUG, "Opened file %s to a buffer", filename);

	buf_clear(b);

	bc = buf_chunk_new(b, f_sz + 1);
	buf_chunk_add(b, bc);

	r = read(fileno(b->fp), bc->buf, bc->size);
	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to read file: %s", filename);

		return r;

	}
	else if (r != f_sz)
	{

		lprintf(LL_ERROR, "Read incorrect size from file: %s", filename);

		return -1;
	}

	for (i = 0; i < bc->size; i++)
	{
		if (bc->buf[i] == '\n' || bc->buf[i] == '\r')
			b->eolcount++;
	}

	if (b->li_count < b->eolcount)
	{
		lprintf(LL_DEBUG, "Current lineinfo amount not enough (%d), allocating %d new li's.", b->li_count, b->eolcount - b->li_count);
		buf_li_add(b, (b->eolcount - b->li_count));

	}
	else
		lprintf(LL_DEBUG, "No need to allocate new lineinfo for %d lines", b->eolcount);

	l_len = 0;
	i = 0;
	c = bc->buf[i];
	c_line = 0;

	while (c)
	{
		if (l_len == 0)
			b->l_info[c_line].p = &bc->buf[i];

		l_len++;

		if (c == '\n' || c == '\r')
		{
			b->l_info[c_line].n = l_len;
			l_len = 0;
			c_line++;
		}

		if (c == '\0')
			break;

		c = bc->buf[++i];
		b->tot_len++;
	}

	lprintf(LL_DEBUG, "Buffer line count: %d", b->eolcount);
	lprintf(LL_DEBUG, "%d bytes read from the file: %s", r, filename);

	return r;
}

void buf_dump_lineinfo(Buffer_t *b)
{
	LineInfo_t *l;
	int i;

	i = 0;

	for (i = 0; i < b->li_count; i++)
	{
		l = &b->l_info[i];
		lprintf(LL_DEBUG, "l_len:%d", l->n);
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
	
	if (b->c_pos >= b->tot_sz)
	{
		lprintf(LL_DEBUG, "Buffer not big enough, allocating a new chunk", b->tot_sz);

		bc = buf_chunk_new(b, BCHUNK_SZ);
		buf_chunk_add(b, bc);
	}

	bc = b->chk_start;
	n = bc->size;
	offset = b->c_pos;

	while (b->c_pos > (n - 1))
	{
		bc = bc->next;
		n += bc->size;
		offset -= bc->size;
	}

	if (b->l_info[b->c_line].n == 0)
		b->l_info[b->c_line].p = &bc->buf[b->c_pos]; 

	b->l_info[b->c_line].n++;

	if (c == '\r' || c == '\n' || b->tot_len == 0)
	{

		if (c == '\r' || c == '\n')
		{
			b->c_line++;
			b->eolcount++;

			if (b->c_line > b->li_count - 1)
				buf_li_add(b, LI_CHUNK_SZ);
		}
	}

	bc->buf[offset] = c;
	b->c_pos++;
	b->tot_len++;

	if (c == 'd')
	{
		buf_dump_lineinfo(b);
	}

	return 0;
}

int buf_clear(Buffer_t *b)
{
	BChunk_t *bc, *tmp;

	bc = b->chk_start;
	while (bc)
	{
		tmp = bc->next;
		buf_chunk_free(b, bc);
		bc = tmp;
	}

	free(b->l_info);

	b->l_info = calloc(LI_CHUNK_SZ, sizeof(LineInfo_t));
	b->li_count = LI_CHUNK_SZ;

	b->c_pos = 0;
	b->eolcount = 0;
	b->tot_sz = 0;
	b->c_line = 0;
	b->chk_start = NULL;

	return 0;
}

