#include <string.h>
#include <stdlib.h>

#include "cmdlib.h"
#include "log.h"

CmdHook_t cmdlib[] = {
	{ FILE_LOAD,   cmdlib_file_load_cb },
	{ FILE_SAVE,   cmdlib_file_save_cb },
	{ MAIN_QUIT,   cmdlib_main_quit_cb },
	{ CURSOR_UP,   cmdlib_cursor_up_cb },
	{ CURSOR_DOWN, cmdlib_cursor_down_cb },
	{ CURSOR_LEFT, cmdlib_cursor_left_cb },
	{ CURSOR_RIGHT, cmdlib_cursor_right_cb },
	{ CMD_LINE,     cmdlib_cmd_line_cb },
	{ CMD_COUNT,   NULL }
};

CmdHook_t *cmdlib_get_lib()
{
	return &cmdlib[0];
}

int cmdlib_file_load_cb(void *uptr, char *args)
{
	Context_t    *ctx;
	int	      r;
	char	     *filename;
	char	     *c;

	ctx = uptr;
	filename = args;

	ncs_clear(ctx->scr);
	buf_clear(ctx->c_buffer);

	lprintf(LL_DEBUG, "Trying to open file: \"%s\"", filename);

	r = buf_load_file(ctx->c_buffer, filename);
	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to load buffer from file: \"%s\"", ctx->c_buffer->filename);
		return r;
	}

	c = buf_get_content(ctx->c_buffer);


	ncs_render_data(ctx->scr, c);

	free(c);

	return 0;
}

int cmdlib_file_save_cb(void *uptr, char *args)
{
	Context_t *ctx;
	int	   r;

	ctx = uptr;

	r = buf_save_file(ctx->c_buffer, ctx->c_buffer->filename);
	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to save buffer to the file", r);
		ed_set_mode(ctx, ED_QUITTING);
	}
	else
		lprintf(LL_DEBUG, "%d bytes written to file", r);

	return r;
}

int cmdlib_cursor_up_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	if (ctx->c_buffer->c_line == 0)
		return 0;

	if (ctx->scr->r_y == 0)
		ncs_scroll(ctx->scr, -1);

	ctx->c_buffer->c_line--;
	ncs_cursor_up(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_down_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	if (ctx->c_buffer->c_line == ctx->c_buffer->linecount - 1)
		return 0;

	if (ctx->c_buffer->c_line >= ctx->scr->h - 1)
	{
		lprintf(LL_DEBUG, "Scrolling down");
		ncs_scroll(ctx->scr, 1);
	}

	ctx->c_buffer->c_line++;
	ncs_cursor_dw(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_left_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_lf(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_right_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	if (!ctx->c_buffer->l_info)
		return 0;

	if (ctx->scr->r_x < ctx->c_buffer->l_info[ctx->c_buffer->c_line].n)
	{
		ncs_cursor_rt(ctx->scr, 1);
	}

	return 0;
}

int cmdlib_cmd_line_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_set_cursor(ctx->scr, 0, ctx->scr->h - 1);
	ncs_addch(ctx->scr, ':');
	buf_add_ch(ctx->cmd_buffer, ':');
	ncs_cursor_rt(ctx->scr, 1);

	ed_set_mode(ctx, ED_CMD_MODE);

	return 0;
}

int cmdlib_main_quit_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;
	ed_set_mode(ctx, ED_QUITTING);

	return 0;
}

