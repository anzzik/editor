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
#include "list.h"

typedef enum CmdID_e CmdID_t;
enum CmdID_e
{
	MAIN_QUIT = 0,
	FILE_LOAD,
	FILE_SAVE,
	CMD_COUNT
};

typedef struct Context_s Context_t;
struct Context_s
{
	InputMode_t  mode;
	Buffer_t    *c_buffer;
	Screen_t    *scr;
	List_t	    *cmd_list;
};

typedef struct CmdHook_s CmdHook_t;
struct CmdHook_s
{
	CmdID_t   cmd_id;
	void (*cmd_hook_cb)(void*);
};

typedef struct CmdLib_s CmdLib_t;
struct CmdLib_s
{
	CmdHook_t hooks[CMD_COUNT];
};

typedef struct Cmd_s Cmd_t;
struct Cmd_s
{
	char	    cmd_id[128];
	char        cmd_str[128];
	void	  (*cmd_hook_cb)(void*);
};


Context_t *ed_new();
int  ed_init(Context_t* ctx);
int  ed_cmd_cmp(void *a, void *b);
int  ed_load_cmd_cfg(Context_t *ctx, const char *cmdfile);
int  ed_cmd_new(Context_t *ctx, char *conf_str);
int  ed_reg_cmd_lib(Context_t *ctx, CmdLib_t *cl);
int  ed_bind_cmd_hook(Context_t *ctx, const char *cmd_id, void (*cb)(void*));
void ed_set_mode(Context_t *ctx, InputMode_t mode);
int  ed_loop(Context_t *ctx);
void ed_info(Context_t *ctx, const char *fmt, ...);
void ed_quit(Context_t *ctx);

#endif

