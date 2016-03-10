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

	ncs_set_cursor(ctx->scr, 0, 0);

	ncs_set_scrolling(ctx->scr, 0);
	ncs_render_data(ctx->scr, c);
	ncs_set_scrolling(ctx->scr, 1);

	ncs_clr_line(ctx->scr, ctx->scr->h - 1);
	ncs_clr_line(ctx->scr, ctx->scr->h - 2);

	ncs_set_cursor(ctx->scr, 0, 0);

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
	int scrl;
	char *buffer;
	LineInfo_t *l;

	ctx = uptr;

	if (ctx->c_buffer->c_line == 0)
		return 0;

	ctx->c_buffer->c_line--;

	l = &ctx->c_buffer->l_info[ctx->c_buffer->c_line];
	if (l->n < ctx->scr->r_x)
		ncs_set_cursor(ctx->scr, l->n, ctx->scr->r_y);

	scrl = ncs_cursor_up(ctx->scr, 1);
	if (scrl)
	{
		buffer = malloc((l->n + 1) * sizeof(char));
		snprintf(buffer, l->n, "%s", l->p);
		buffer[l->n] = '\0';


		ncs_scroll(ctx->scr, scrl);
		ncs_addstrf(ctx->scr, 0, 0, "%s", buffer);

		ncs_clr_line(ctx->scr, ctx->scr->h - 1);
		ncs_clr_line(ctx->scr, ctx->scr->h - 2);

		free(buffer);
	}

	ncs_clr_line(ctx->scr, ctx->scr->h - 1);
	ncs_addstrf(ctx->scr, 0, ctx->scr->h - 1, 
			"Line: %d, len: %d", ctx->c_buffer->c_line + 1, ctx->c_buffer->l_info[ctx->c_buffer->c_line].n);

	return 0;
}

int cmdlib_cursor_down_cb(void *uptr, char *args)
{
	Context_t *ctx;
	int scrl, n;
	char *buffer, *p;
	LineInfo_t *l;

	ctx = uptr;

	if (ctx->c_buffer->c_line + 1 >= ctx->c_buffer->eolcount)
		return 0;

	if (ctx->c_buffer->c_line + 1 > ctx->c_buffer->li_count)
	{
		lprintf(LL_ERROR, "Not enough lineinfo allocated!");
		return -1;
	}

	ctx->c_buffer->c_line++;

	l = &ctx->c_buffer->l_info[ctx->c_buffer->c_line];

	if (l->n < ctx->scr->r_x)
		ncs_set_cursor(ctx->scr, l->n, ctx->scr->r_y);

	scrl = ncs_cursor_dw(ctx->scr, 1);
	if (scrl)
	{
		n = ctx->c_buffer->l_info[ctx->c_buffer->c_line].n;
		p = ctx->c_buffer->l_info[ctx->c_buffer->c_line].p;

		buffer = malloc((n + 1) * sizeof(char));
		snprintf(buffer, n, "%s", p);
		buffer[n] = '\0';

		ncs_scroll(ctx->scr, scrl);
		ncs_addstrf(ctx->scr, 0, ctx->scr->h - 3, "%s", buffer);

		ncs_clr_line(ctx->scr, ctx->scr->h - 1);
		ncs_clr_line(ctx->scr, ctx->scr->h - 2);


		free(buffer);
	}

	ncs_clr_line(ctx->scr, ctx->scr->h - 1);
	ncs_addstrf(ctx->scr, 0, ctx->scr->h - 1, 
			"Line: %d, len: %d, eolcount: %d", 
			ctx->c_buffer->c_line + 1, ctx->c_buffer->l_info[ctx->c_buffer->c_line].n,
			ctx->c_buffer->eolcount);

	return 0;
}

int cmdlib_cursor_left_cb(void *uptr, char *args)
{
	Context_t *ctx;
	char c;

	ctx = uptr;

	if (!ctx->c_buffer->l_info)
		return 0;

	c = buf_get_char(ctx->c_buffer, ctx->c_buffer->c_line, ctx->c_buffer->c_col);
	if (ctx->c_buffer->c_col > 0)
	{
		if (c == '\t')
			ncs_cursor_lf(ctx->scr, 8);
		else
			ncs_cursor_lf(ctx->scr, 1);


		ctx->c_buffer->c_col--;
	}

	return 0;
}

int cmdlib_cursor_right_cb(void *uptr, char *args)
{
	Context_t *ctx;
	char c;

	ctx = uptr;

	if (!ctx->c_buffer->l_info)
		return 0;

	c = buf_get_char(ctx->c_buffer, ctx->c_buffer->c_line, ctx->c_buffer->c_col);

	//if (ctx->scr->r_x < ctx->c_buffer->l_info[ctx->c_buffer->c_line].n - 1)
	if (c != '\n' && c != '\r' && c != '\0')
	{
		if (c == '\t')
			ncs_cursor_rt(ctx->scr, 8);
		else
			ncs_cursor_rt(ctx->scr, 1);

		ctx->c_buffer->c_col++;
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

