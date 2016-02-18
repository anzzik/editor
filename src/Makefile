root_dir = ../
obj_dir = $(root_dir)obj/
deps_dir = $(root_dir)dep/
bin_dir = $(root_dir)bin/
binary = $(bin_dir)test

CC = gcc
CFLAGS = -O0 -g -Wall -D_GNU_SOURCE=1 -D_REENTRANT -I ./
LDFLAGS = 
LIBS = -lcurses

sources = \
	main.c \
	editor.c \
	screen.c \
	input.c \
	input_cmd.c \
	buffer.c \

OBJS = $(sources:%.c=$(obj_dir)%.o)
deps = $(sources:%.c=$(deps_dir)%.d)

all: $(binary)
$(binary): $(OBJS) $(GOBJS)
	@mkdir -p $(bin_dir) 
	@echo "linking to $@"
	@$(CC) $(LDFLAGS) $(OBJS) $(GOBJS) -o $(binary) $(LIBS)

$(obj_dir)%.o: %.c
	@mkdir -p $(obj_dir)
	@echo "compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

ifneq ($(MAKECMDGOALS),clean)
-include $(deps)
endif

$(deps_dir)%.d: %.c
	@mkdir -p $(deps_dir)
	@$(SHELL) -ec '$(CC) -MM $(CFLAGS) $(CPPFLAGS) $< \
	| sed "s/$*\\.o[ :]*/$(subst /,\\/,$@) &/g" | sed "s/$*\\.o[ :]*/$(subst /,\\/,$(obj_dir))&/" > $@'

.PHONY : clean
clean: 
	rm -rf $(obj_dir)
	rm -rf $(deps_dir)
	rm -rf $(bin_dir)

