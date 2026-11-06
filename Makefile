NAME        = lib/valgrind_lib.so
SRC         = src/valgrind_hook.c
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -fPIC -O3
LDFLAGS     = -shared -ldl

PREFIX_BIN  = $(HOME)/.local/bin
PREFIX_LIB  = $(HOME)/.local/lib

all: $(NAME)

$(NAME): $(SRC)
	@mkdir -p lib
	$(CC) $(CFLAGS) $(SRC) -o $(NAME) $(LDFLAGS)
	@echo "Build complete: $(NAME)"

install: all
	@mkdir -p $(PREFIX_BIN) $(PREFIX_LIB)
	@cp $(NAME) $(PREFIX_LIB)/valgrind_lib.so
	@cp bin/valgrind $(PREFIX_BIN)/valgrind
	@chmod +x $(PREFIX_BIN)/valgrind
	@echo "Installed valgrind to $(PREFIX_BIN)/valgrind"
	@echo "Installed library to $(PREFIX_LIB)/valgrind_lib.so"
	@echo "Make sure $(PREFIX_BIN) is in your PATH."

uninstall:
	@rm -f $(PREFIX_BIN)/valgrind
	@rm -f $(PREFIX_LIB)/valgrind_lib.so
	@echo "Uninstalled valgrind."

clean:
	@rm -rf lib

fclean: clean

re: fclean all

.PHONY: all install uninstall clean fclean re
