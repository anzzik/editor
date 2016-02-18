#ifndef EDITOR_H
#define EDITOR_H

#include "input.h"
#include "screen.h"
#include "buffer.h"

typedef struct Context_s Context_t;
struct Context_s
{
	InputMode_t  mode;
	Buffer_t    *c_buffer;
	Screen_t    *scr;
};

Context_t *ed_new();
int  ed_init(Context_t* ctx);
void ed_set_mode(Context_t *ctx, InputMode_t mode);
int  ed_loop(Context_t *ctx);
void ed_info(Context_t *ctx, const char *fmt, ...);
void ed_quit(Context_t *ctx);

#endif

