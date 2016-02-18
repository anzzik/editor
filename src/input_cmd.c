#include <stdio.h>
#include "input_cmd.h"


int input_cmd_key_wait(void *uptr)
{
	int c;

	c = getchar();

	return c;
}

int input_cmd_key_down(void *uptr, int c)
{
	switch (c)
	{
		case 'f':
			break;

		case 'q':
			break;

		case 'h':
			break;

		case 'j':
			break;

		case 'l':
			break;

		case 'i':
			break;

		default:
			break;
	}

	return 0;
}

