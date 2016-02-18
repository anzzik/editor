#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

static InputModeInfo_t *modes[MODE_COUNT] = { NULL };
static InputMode_t current_mode = CMD_MODE;

void input_init()
{
	int i;

	for (i = 0; i < MODE_COUNT; i++)
	{
		modes[i] = input_mode_new(i);
	}
}

InputModeInfo_t *input_mode_new(InputMode_t mode)
{
	InputModeInfo_t *im;

	im = malloc(sizeof(InputModeInfo_t));
	im->next = 0;
	im->prev = 0;
	im->mode = mode;
	im->lib = malloc(sizeof(InputLib_t));

	return im;
}

int input_register_lib(InputMode_t mode, InputLib_t* il)
{
	if (!modes[mode])
	{
		fprintf(stderr, "lib register failed: mode is not initialized\n");

		return -1;
	}

	memcpy(modes[mode]->lib, il, sizeof(InputLib_t));

	return 0;
}

int input_key_down(void *uptr, int c)
{
	if (!modes[current_mode])
	{
		fprintf(stderr, "mode is not initialized, input hook missing\n");

		return -1;
	}

	modes[current_mode]->lib->on_key_down_cb(uptr, c);

	return 0;
}

int input_key_wait(void *uptr)
{
	int r;

	if (!modes[current_mode])
	{
		fprintf(stderr, "mode is not initialized, input hook missing\n");

		return -1;
	}

	r = modes[current_mode]->lib->on_key_wait_cb(uptr);

	return r;
}

void input_quit()
{
	int i;

	for (i = 0; i < MODE_COUNT; i++)
	{
		free(modes[i]->lib);
		free(modes[i]);
	}
}


