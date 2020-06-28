/*
   Copyright (C) 2020 Anssi Kulju 

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

#ifndef CMDLIB_H
#define CMDLIB_H

#include "editor.h"


CmdHook_t *cmdlib_get_lib();
CmdLib_t *cmdlib_get_rlib();
int cmdlib_file_load_cb(void *uptr, char *args);
int cmdlib_file_save_cb(void *uptr, char *args);
int cmdlib_cursor_up_cb(void *uptr, char *args);
int cmdlib_cursor_down_cb(void *uptr, char *args);
int cmdlib_cursor_left_cb(void *uptr, char *args);
int cmdlib_cursor_right_cb(void *uptr, char *args);
int cmdlib_cmd_line_cb(void *uptr, char *args);
int cmdlib_main_quit_cb(void *uptr, char *args);

#endif

