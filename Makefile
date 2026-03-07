CC = gcc
CFLAGS = -Wall -Wextra -g
SANITIZE = -fsanitize=address

all: mdcat

clean:
	rm -rf mdcat mdcat_sanitize mdcat.dSYM mdcat_sanitize.dSYM
	@cd test && $(MAKE) clean

mdcat: mdcat.c
	$(CC) $(CFLAGS) -o mdcat mdcat.c

sanitize: mdcat.c
	$(CC) $(CFLAGS) $(SANITIZE) -o mdcat_sanitize mdcat.c

test: all
	@cd test && $(MAKE)

test-sanitize: sanitize
	@cd test && MDCAT_BINARY=../mdcat_sanitize sh runall.sh

format:
	clang-format -i *.c

.PHONY: format
