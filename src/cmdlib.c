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

int cmdlib_file_load_cb(void *uptr)
{
	Context_t *ctx;
	BChunk_t  *bc;
	int r;
	char *cmd_buf;

	ctx = uptr;
	
	cmd_buf = buf_get_content(ctx->cmd_buffer);

	r = buf_load_file(ctx->c_buffer, "screen.c");
	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to load buffer from file: %s", ctx->c_buffer->filename);
		return r;
	}

	ncs_set_cursor(ctx->scr, 0, 0);

	bc = ctx->c_buffer->chk_start;
	while (bc)
	{
		ncs_render_data(ctx->scr, bc->buf);
		bc = bc->next;
	}

	ncs_set_cursor(ctx->scr, 0, 0);

	return 0;
}

int cmdlib_file_save_cb(void *uptr)
{
	Context_t *ctx;
	int	   r;

	ctx = uptr;

	r = buf_save_file(ctx->c_buffer, ctx->c_buffer->filename);

	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to save buffer to the file", r);
		ed_set_edmode(ctx, ED_QUITTING);
	}
	else
		lprintf(LL_DEBUG, "%d bytes written to file", r);

	ncs_set_cursor(ctx->scr, 0, 0);

	return r;
}

int cmdlib_cursor_up_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_up(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_down_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_dw(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_left_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_lf(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_right_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_rt(ctx->scr, 1);

	return 0;
}

int cmdlib_cmd_line_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_set_cursor(ctx->scr, 0, ctx->scr->h - 2);
	ncs_addch(ctx->scr, ':');
	ncs_cursor_rt(ctx->scr, 1);

	ed_set_mode(ctx, CMD_MODE);

	return 0;
}

int cmdlib_main_quit_cb(void *uptr)
{
	Context_t *ctx;

	ctx = uptr;
	ed_set_edmode(ctx, ED_QUITTING);

	return 0;
}
