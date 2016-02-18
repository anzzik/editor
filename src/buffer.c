#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

Buffer_t *buf_new()
{
	Buffer_t *b;
	BChunk_t *bc;

	b = malloc(sizeof(Buffer_t));
	b->filename = "scratch.txt";

	bc = buf_chunk_new(b);
	buf_chunk_add(b, bc);
	
	return b;
}

BChunk_t *buf_chunk_new(Buffer_t *b)
{
	BChunk_t *bc;

	bc = malloc(sizeof(BChunk_t));

	memset(bc->buf, '\0', sizeof(bc->buf));
	bc->next = 0;
	bc->prev = 0;

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

int buf_load_file(Buffer_t *b, const char *filename)
{
	FILE	  *fp;
	int	   r, i;
	BChunk_t  *bc;

	fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "Cannot open file: %s\n", filename);
		return -1;
	}

	bc = b->chk_start;
	while (1)
	{
		r = read(fileno(fp), bc->buf, (sizeof(bc->buf) - 1));
		if (r < 0)
		{
			fprintf(stderr, "read() failed in file: %s\n", filename);
			fclose(fp);

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

	fclose(fp);

	return 0;
}

int buf_save_file(Buffer_t *b, const char *filename)
{
	FILE	  *fp;
	int	   r;
	BChunk_t  *bc;

	fp = fopen(filename, "w");
	if (!fp)
	{
		fprintf(stderr, "Cannot open file: %s\n", filename);
		return -1;
	}

	bc = b->chk_start;
	while (bc)
	{
		r = write(fileno(fp), bc->buf, (sizeof(bc->buf) - 1));
		if (r < 0)
		{
			fprintf(stderr, "write() failed in file: %s\n", filename);

			return -1;
		}

		if (r == 0)
			break;

		bc = bc->next;
	}

	return 0;
}

