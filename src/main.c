#include <stdio.h>
#include <string.h>

#include "editor.h"

int main(void)
{
	Context_t *ctx = ed_new();
	ed_init(ctx);
	ed_loop(ctx);
	ed_quit(ctx);
	ed_free(ctx);

	return 0;
}

