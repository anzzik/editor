#ifndef BUFFER_H
#define BUFFER_H

#define BCHUNK_SZ 128

typedef struct BChunk_s BChunk_t;
struct BChunk_s
{
	char buf[BCHUNK_SZ];

	BChunk_t *next, *prev;
};

typedef struct Buffer_s Buffer_t;
struct Buffer_s
{
	char *name;
	BChunk_t *chk_start;
	const char *filename;
};

Buffer_t *buf_new();
BChunk_t *buf_chunk_new(Buffer_t *b);
int	  buf_chunk_add(Buffer_t *b, BChunk_t *bc);
void	  buf_chunk_close(Buffer_t* b, BChunk_t *bc);
void	  buf_close(Buffer_t *b);
int	  buf_load_file(Buffer_t *b, const char *filename);
int	  buf_save_file(Buffer_t *b, const char *filename);

#endif

