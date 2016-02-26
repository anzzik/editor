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

#ifndef EDITOR_H
#define EDITOR_H

#include "input.h"
#include "screen.h"
#include "buffer.h"

typedef enum CmdType_e CmdType_t;
enum CmdType_e
{
	MAIN_QUIT = 0,
	FILE_LOAD, /* 1 */
	FILE_SAVE, /* 2 */
	CURSOR_UP,
	CURSOR_DOWN,
	CURSOR_LEFT,
	CURSOR_RIGHT,
	CMD_LINE,
	CMD_COUNT
};


typedef enum EdMode_e EdMode_t;
enum EdMode_e
{
	ED_RUNNING = 0,
	ED_QUITTING
};


typedef struct Context_s Context_t;
struct Context_s
{
	char	     pending_info[128];
	EdMode_t     ed_mode;
	InputMode_t  mode;
	Buffer_t    *c_buffer;
	Buffer_t    *cmd_buffer;
	Screen_t    *scr;
};

typedef struct Cmd_s Cmd_t;
struct Cmd_s
{
	CmdType_t   cmd_type;
	char	    cmd_id[128];
	char        cmd_str[128];
	char        cmd_hk[128];
	int	  (*cmd_cb)(void*);
};

typedef struct CmdHook_s CmdHook_t;
struct CmdHook_s
{
	CmdType_t type;
	int (*cmd_hook_cb)(void*);
};


Context_t *ed_new();
int  ed_init(Context_t* ctx);
int  ed_cmd_cmp(void *a, void *b);
void ed_dump_cmds();
Cmd_t *ed_get_cmd_by_hk(int c);
int  ed_load_cmd_cfg(Context_t *ctx, const char *cmdfile);
int  ed_cmd_parse_cfg(Context_t *ctx, char *conf_str);
int  ed_bind_cmd_hook(Context_t *ctx, CmdType_t cmd_type, int (*cb)(void*));
void ed_set_mode(Context_t *ctx, InputMode_t mode);
void ed_set_edmode(Context_t *ctx, EdMode_t mode);
int  ed_loop(Context_t *ctx);
void ed_info(Context_t *ctx, const char *fmt, ...);
void ed_quit(Context_t *ctx);

#endif

