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
#include "cmdlib.h"
#include "log.h"

Cmd_t cmds[] = 
{
	{ MAIN_QUIT,    "main_quit", ":q", "00", NULL },
	{ FILE_LOAD,    "file_load", ":e", "00", NULL },
	{ FILE_SAVE,    "file_save", ":w", "00", NULL },
	{ CURSOR_UP,    "cursor_up", ":w", "00", NULL },
	{ CURSOR_DOWN,  "cursor_down", ":w", "00", NULL },
	{ CURSOR_LEFT,  "cursor_left", ":w", "00", NULL },
	{ CURSOR_RIGHT, "cursor_right", ":w", "00", NULL },
	{ CMD_LINE,	"cmd_line", ":", ":", NULL },

	{ CMD_COUNT,	"", "", "", NULL },
};

void ed_dump_cmds()
{
	int i;

	for (i = 0; i < CMD_COUNT; i++)
	{
		printf("%d, %s, %s, %s, %p\n",
				cmds[i].cmd_type,
				cmds[i].cmd_id,
				cmds[i].cmd_str,
				cmds[i].cmd_hk,
				cmds[i].cmd_cb);
	}
}

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

CmdType_t ed_cmd_get_type(const char *cmd_id)
{
	int i;

	for (i = 0; i < CMD_COUNT; i++)
	{
		if (!strcmp(cmds[i].cmd_id, cmd_id))
			return cmds[i].cmd_type;
	}

	return -1;
}

Cmd_t *ed_get_cmd(char *cmd)
{
	int i;

	if (!cmd)
	{
		lprintf(LL_ERROR, "Cmdline empty in ed_get_cmd");
		return NULL;
	}

	for (i = 0; i < CMD_COUNT; i++)
	{
		if (!strcmp(cmds[i].cmd_str, cmd))
			return &cmds[i];
	}

	return NULL;
}

Cmd_t *ed_get_cmd_by_cmdstr(const char *cmd_str)
{
	int i;
	char cmd[128];

	memset(&cmd, 0, sizeof(cmd));
	for (i = 0; i < CMD_COUNT; i++)
	{
		memcpy(cmd, cmd_str, strlen(cmds[i].cmd_str));

		if (!strcmp(cmds[i].cmd_str, cmd))
			return &cmds[i];

		memset(&cmd, 0, sizeof(cmd));
	}

	return NULL;
}

int ed_init(Context_t* ctx)
{
	CmdHook_t *hooks;
	int i;

	/* logging*/
	log_add(LL_ERROR, "error.log");
	log_add(LL_DEBUG, "debug.log");

	/* screen */
	ctx->scr = ncs_new();
	ncs_init(ctx->scr);

	/* buffer */
	ctx->c_buffer = buf_new("scratch.txt");
	ctx->cmd_buffer = buf_new("cmdbuf.txt");

	/* main context */
	ed_set_mode(ctx, ED_HOTKEY_MODE);
	ed_load_cmd_cfg(ctx, "cmds.txt");

	hooks = cmdlib_get_lib();

	i = 0;

	while (hooks[i].type != CMD_COUNT)
	{
		ed_bind_cmd_hook(ctx, hooks[i].type, hooks[i].cmd_hook_cb);
		i++;
	}

	return 0;
}

int ed_bind_cmd_hook(Context_t *ctx, CmdType_t cmd_type, int (*cb)(void*, char*))
{
	if (cmd_type >= CMD_COUNT)
	{
		lprintf(LL_ERROR, "cmd type wrong\n");
		return -1;
	}

	cmds[cmd_type].cmd_cb = cb;

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
		lprintf(LL_ERROR, "Cannot open file: %s\n", cfgfile);
		return -1;
	}

	r = fseek(fp, 0, SEEK_END);
	if (r)
	{
		lprintf(LL_ERROR, "fseek failed: %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	f_sz = ftell(fp);
	if (f_sz == -1L)
	{
		lprintf(LL_ERROR, "ftell failed: %s\n", cfgfile);
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
		lprintf(LL_ERROR, "read() 0 bytes read from %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	if (r < 0)
	{
		lprintf(LL_ERROR, "read() failed in file: %s\n", cfgfile);
		fclose(fp);

		return -1;
	}

	if (r != f_sz)
	{
		lprintf(LL_ERROR, "Couldn't read() the whole file: %s, %d != %d\n", cfgfile, r, f_sz);
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
				ed_cmd_parse_cfg(ctx, tmp);

			default:
				tmp[strlen(tmp)] = c;
		}

		c = buf[++i];
	}

	free(buf);

	return 0;
}

int ed_cmd_parse_cfg(Context_t *ctx, char *conf_str)
{
	CmdType_t type;
	char id[128];
	char cmd_str[256];
	char tmp[20];
	char hotkey[20];

	sscanf(conf_str, "%[^';'];%[^';'];%[^';'];", id, cmd_str, hotkey);
	//printf("Creating cmd with id: %s and cmd_str: %s and hotkey: %s\n", id, cmd_str, hotkey);

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, id, 3);

	if (strcmp(tmp, "id="))
	{
		lprintf(LL_ERROR, "id wrong\n");
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, cmd_str, 4);

	if (strcmp(tmp, "cmd="))
	{
		lprintf(LL_ERROR, "cmd_str wrong\n");
		return -1;
	}

	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, hotkey, 7);

	if (strcmp(tmp, "hotkey="))
	{
		lprintf(LL_ERROR, "hotkey wrong\n");
		return -1;
	}

	type = ed_cmd_get_type(id + 3);

	strncpy(cmds[type].cmd_str, cmd_str + 4, sizeof(cmds[type].cmd_str));
	strncpy(cmds[type].cmd_hk,  hotkey  + 7, sizeof(cmds[type].cmd_str));

	return 0;
}

void ed_set_mode(Context_t *ctx, EdMode_t mode)
{
	ctx->mode = mode;
	ed_info(ctx, "Mode: %d", ctx->mode);
}

Cmd_t *ed_get_cmd_by_hk(c)
{
	char *ptr;
	int   i;

	for (i = 0; i < CMD_COUNT; i++)
	{
		if (strtol(cmds[i].cmd_hk, &ptr, 10) == c)
		{
			return &cmds[i];
		}

		if (cmds[i].cmd_hk[0] == c)
		{
			return &cmds[i];
		}
	}

	return NULL;
}

int ed_loop(Context_t *ctx)
{
	Cmd_t    *cmd;
	int	  r;
	char	  c;

	while (1)
	{
		if (ctx->mode == ED_QUITTING)
			break;

		c = getchar();
		switch (ctx->mode)
		{
			case ED_HOTKEY_MODE:
				cmd = ed_get_cmd_by_hk(c);
				if (cmd)
				{
					r = cmd->cmd_cb(ctx, NULL);
					if (!r)
						break;

					lprintf(LL_CRITICAL, "Command \"%s\" was not executed succesfully", cmd->cmd_id);

					return -1;
				}

				switch (c)
				{
					case 'i':
						ed_set_mode(ctx, ED_INSERT_MODE);

						break;
				}

				break;

			case ED_CMD_MODE:

				if (c == '\r' || c == '\n')
				{
					buf_add_ch(ctx->cmd_buffer, '\0');
					ed_parse_cmd_buf(ctx);

					buf_clear(ctx->cmd_buffer);

					ed_set_mode(ctx, ED_HOTKEY_MODE);
					ncs_set_cursor(ctx->scr, 0, 0);
					break;
				}

				buf_add_ch(ctx->cmd_buffer, c);

				ncs_addch(ctx->scr, c);
				ncs_cursor_rt(ctx->scr, 1);

				break;

			case ED_INSERT_MODE:

				if (c == 27)
				{
					ed_set_mode(ctx, ED_HOTKEY_MODE);
					break;
				}

				if (c & (0x1 << 7))
				{
					ed_info(ctx, "Non-ASCII characters not supported");
					break;
				}

				if (c == '\r' || c == '\n')
				{
					ncs_set_cursor(ctx->scr, 0, ctx->scr->c_y + 1);
				}

				if (c == '\t')
				{
					ncs_cursor_rt(ctx->scr, 8);
				}

				buf_add_ch(ctx->c_buffer, c);

				ncs_addch(ctx->scr, c);
				ncs_cursor_rt(ctx->scr, 1);

				break;

			default:
				lprintf(LL_ERROR, "Mode not set!\n");
				break;
		}
	}

	return 0;
}

int ed_parse_cmd_buf(Context_t *ctx)
{
	Cmd_t *cmd;
	char  *cmdline;
	char  *c;
	char  *tok;

	cmdline = buf_get_content(ctx->cmd_buffer);
	c = malloc(strlen(cmdline) + 1);
	strncpy(c, cmdline, strlen(cmdline) + 1);

	tok = strtok(c, " ");

	cmd = ed_get_cmd(tok);
	free(c);
	if (cmd == NULL)
	{
		lprintf(LL_NOTICE, "Command %s not found", tok);
		free(cmdline);

		return -1;
	}

	lprintf(LL_NOTICE, "Command %s loaded", cmd->cmd_id);

	cmd->cmd_cb(ctx, cmdline + strlen(tok)+1);

	free(cmdline);

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
	ncs_addstr(ctx->scr, 0, ctx->scr->h - 2, "%*s", 20, " ");
	ncs_addstr(ctx->scr, 0, ctx->scr->h - 2, "%s", buffer);
	ncs_stop_color(ctx->scr, 2);

	ncs_cursor_revert(ctx->scr);
}

void ed_quit(Context_t *ctx)
{
	buf_free(ctx->c_buffer);
	buf_free(ctx->cmd_buffer);
	ncs_close(ctx->scr);
	ncs_quit();
	log_quit();
}

void ed_free(Context_t* ctx)
{
	free(ctx);
}


