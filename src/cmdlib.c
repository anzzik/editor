#include <string.h>
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

/*CmdLib_t r_cmdlib = {
	cmdlib_file_load_cb,
	cmdlib_file_save_cb,
	cmdlib_main_quit_cb,
	cmdlib_cursor_up_cb,
	cmdlib_cursor_down_cb,
	cmdlib_cursor_left_cb,
	cmdlib_cursor_right_cb,
	cmdlib_cmd_line_cb
};

CmdLib_t *cmdlib_get_rlib()
{
	return &r_cmdlib;
}*/

CmdHook_t *cmdlib_get_lib()
{
	return &cmdlib[0];
}

int cmdlib_file_load_cb(void *uptr, char *args)
{
	Context_t *ctx;
	BChunk_t  *bc;
	int r;
	char def_fname[128];

	ctx = uptr;

	if (args)
		strncpy(def_fname, args, sizeof(def_fname));
	else
		strncpy(def_fname, "scratch", sizeof(def_fname));
	
	ncs_clear(ctx->scr);

	r = buf_load_file(ctx->c_buffer, def_fname);
	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to load buffer from file: %s|", ctx->c_buffer->filename);
		return r;
	}

	ncs_set_cursor(ctx->scr, 0, 0);

	ncs_clear(ctx->scr);
	bc = ctx->c_buffer->chk_start;
	while (bc)
	{
		ncs_render_data(ctx->scr, bc->buf);
		bc = bc->next;
	}

	ncs_set_cursor(ctx->scr, 0, 0);

	return 0;
}

int cmdlib_file_save_cb(void *uptr, char *args)
{
	Context_t *ctx;
	int	   r;

	ctx = uptr;

	if (args)
		strncpy(ctx->c_buffer->filename, args, sizeof(*ctx->c_buffer->filename));

	r = buf_save_file(ctx->c_buffer, ctx->c_buffer->filename);

	if (r < 0)
	{
		lprintf(LL_ERROR, "Failed to save buffer to the file", r);
		ed_set_mode(ctx, ED_QUITTING);
	}
	else
		lprintf(LL_DEBUG, "%d bytes written to file", r);

	ncs_set_cursor(ctx->scr, 0, 0);

	return r;
}

int cmdlib_cursor_up_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

	ncs_cursor_up(ctx->scr, 1);

	return 0;
}

int cmdlib_cursor_down_cb(void *uptr, char *args)
{
	Context_t *ctx;

	ctx = uptr;

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

	ncs_cursor_rt(ctx->scr, 1);

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

