#ifndef CMDLIB_H
#define CMDLIB_H

#include "editor.h"


CmdHook_t *cmdlib_get_lib();
int cmdlib_file_load_cb(void *uptr);
int cmdlib_file_save_cb(void *uptr);
int cmdlib_cursor_up_cb(void *uptr);
int cmdlib_cursor_down_cb(void *uptr);
int cmdlib_cursor_left_cb(void *uptr);
int cmdlib_cursor_right_cb(void *uptr);
int cmdlib_cmd_line_cb(void *uptr);
int cmdlib_main_quit_cb(void *uptr);

#endif

