#ifndef INPUT_H
#define INPUT_H

typedef enum InputMode_e InputMode_t;
enum InputMode_e
{
	CMD_MODE = 0,
	INSERT_MODE,
	MODE_COUNT
};

typedef struct InputLib_s InputLib_t;
struct InputLib_s
{
	int (*on_key_down_cb)(void*, int c);
	int (*on_key_wait_cb)(void*);
};

typedef struct InputModeInfo_s InputModeInfo_t;
struct InputModeInfo_s
{
	InputMode_t mode;
	InputLib_t *lib;

	InputModeInfo_t *next, *prev;
};

void input_init();
InputModeInfo_t *input_mode_new(InputMode_t mode);
int input_register_lib(InputMode_t mode, InputLib_t* il);
int input_key_down(void *uptr, int c);
int input_key_wait(void *uptr);

#endif

