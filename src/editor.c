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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "editor.h"
#include "input_cmd.h"

Context_t *ed_new()
{
	Context_t *c = malloc(sizeof(Context_t));

	return c;
}

int ed_cmd_cmp(void *a, void *b)
{
	Cmd_t *aa, *bb;

	aa = a;
	bb = b;

	return strcmp(aa->cmd_id, bb->cmd_id);
}

int ed_init(Context_t* ctx)
{
	InputLib_t il;

	/* main context */
	ed_set_mode(ctx, CMD_MODE);
	ctx->cmd_list = ls_new(ed_cmd_cmp);

	ed_load_cmd_cfg(ctx, "cmds.txt");
	
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

int ed_bind_cmd_hook(Context_t *ctx, const char *cmd_id, void (*cb)(void*))
{
	return 0;
}

int ed_load_cmd_cfg(Context_t *ctx, const char *cfgfile)
{
	FILE *fp;
	int   f_sz, r, i;
	char *buf;
	char  tmp[256] = {'\0'};
	char  c;

	fp = fopen(cfgfile, "r");
	if (!fp)
	{
		fprintf(stderr, "Cannot open file: %s\n", cfgfile);
		return -1;
	}

	r = fseek(fp, 0, SEEK_END);
	if (r)
	{
		fprintf(stderr, "fseek failed: %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	f_sz = ftell(fp);
	if (f_sz == -1L)
	{
		fprintf(stderr, "ftell failed: %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	rewind(fp);
	fflush(fp);

	buf = malloc(f_sz);
	memset(buf, 0, f_sz);

	r = read(fileno(fp), buf, f_sz);
	if (r == 0)
	{
		fprintf(stderr, "read() 0 bytes read from %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	if (r < 0)
	{
		fprintf(stderr, "read() failed in file: %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	if (r != f_sz)
	{
		fprintf(stderr, "Couldn't read() the whole file: %s, %d != %d\n", cfgfile, r, f_sz);
		fclose(fp);

		return -1;
	}


	i = 0;

	c = buf[i];
	while (c)
	{
		if (isspace(c))
		{
			c = buf[++i];
			continue;
		}

		switch (c)
		{
			case '{': 
				memset(tmp, 0, sizeof(tmp));
				break;

			case '}':
				ed_cmd_new(ctx, tmp);

			default:
				tmp[strlen(tmp)] = c;
		}

		c = buf[++i];
	}

	return 0;
}

int ed_reg_cmd_lib(Context_t *ctx, CmdLib_t *cl)
{
}

int ed_cmd_new(Context_t *ctx, char *conf_str)
{
	Cmd_t *cmd;
	char id[128];
	char cmd_str[256];
	char tmp[10];

	sscanf(conf_str, "%[^';'];%[^';'];", id, cmd_str);
	printf("Creating cmd with id: %s and cmd_str: %s\n", id, cmd_str);

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, id, 3);

	if (strcmp(tmp, "id="))
	{
		fprintf(stderr, "id wrong\n");
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, cmd_str, 4);

	if (strcmp(tmp, "cmd="))
	{
		fprintf(stderr, "id wrong\n");
		return -1;
	}

	cmd = malloc(sizeof(Cmd_t));

	strncpy(cmd->cmd_id,  id + 3, sizeof(cmd->cmd_id));
	strncpy(cmd->cmd_str, cmd_str + 4, sizeof(cmd->cmd_id));

	ls_add(ctx->cmd_list, cmd);

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

