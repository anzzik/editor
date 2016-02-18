#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "input_cmd.h"

Context_t *ed_new()
{
	Context_t *c = malloc(sizeof(Context_t));

	return c;
}

int ed_init(Context_t* ctx)
{
	InputLib_t il;

	/* main context */
	ed_set_mode(ctx, CMD_MODE);
	
	/* screen */
	ctx->scr = ncs_new();
	ncs_init(ctx->scr);

	/* buffer */
	ctx->c_buffer = buf_new();

	/* input */
	input_init();
	il.on_key_down_cb = input_cmd_key_down;
	il.on_key_wait_cb = input_cmd_key_wait;

	input_register_lib(CMD_MODE, &il);

	return 0;
}

void ed_set_mode(Context_t *ctx, InputMode_t mode)
{
	ctx->mode = mode;
}

int ed_loop(Context_t *ctx)
{
	BChunk_t *bc;
	int q, c, r;

	q = 0;
	while (1)
	{
		switch (ctx->mode)
		{
			case CMD_MODE:

				c = input_key_wait(ctx);
				switch (c)
				{
					case 'q':
						q = 1;

						break;

					case 'f':

						buf_load_file(ctx->c_buffer, "screen.c");
						ncs_set_cursor(ctx->scr, 0, 0);

						bc = ctx->c_buffer->chk_start;
						while (bc)
						{
							ncs_render_data(ctx->scr, bc->buf);
							bc = bc->next;
						}

						ncs_set_cursor(ctx->scr, 0, 0);

						break;

					case 's':
						r = buf_save_file(ctx->c_buffer, "testsave.c");
						if (r < 0)
							q = 1;

						break;

					case 'i':
						ed_set_mode(ctx, INSERT_MODE);

						break;

					case 'h':
						ncs_cursor_lf(ctx->scr, 1);
						break;

					case 'j':
						ncs_cursor_dw(ctx->scr, 1);
						break;

					case 'k':
						ncs_cursor_up(ctx->scr, 1);
						break;

					case 'l':
						ncs_cursor_rt(ctx->scr, 1);
						break;

				}

				break;

			case INSERT_MODE:
				c = input_key_wait(ctx);
				if (c == 27)
				{
					ed_set_mode(ctx, CMD_MODE);
					break;
				}

				if (c == '\r' || c == '\n')
				{
					ncs_set_cursor(ctx->scr, 0, ctx->scr->c_y + 1);
					break;
				}

				if (c == '\t')
				{
					ncs_cursor_rt(ctx->scr, 8);
					break;
				}

				if (c & (0x1 << 7))
				{
					ed_info(ctx, "Non-ASCII characters not supported");
					break;
				}

				ncs_addch(ctx->scr, c);
				ncs_cursor_rt(ctx->scr, 1);

				break;

			default:
				fprintf(stderr, "mode not set!\n");
				break;
		}

		if (q)
			break;
	}

	return 0;
}

void ed_info(Context_t *ctx, const char *fmt, ...)
{
	va_list ap;
	char	buffer[256] = {'\0'};

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	ncs_start_color(ctx->scr, 2);
	ncs_addstr(ctx->scr, 0, ctx->scr->h - 2, strlen(buffer), "%s", buffer);
	ncs_stop_color(ctx->scr, 2);

	ncs_cursor_revert(ctx->scr);
}

void ed_quit(Context_t *ctx)
{
	ncs_close(ctx->scr);
	ncs_quit();
}

