CC = gcc
CFLAGS = -Wall -Wextra -g
SANITIZE = -fsanitize=address

all: mdcat

clean:
	rm -rf mdcat mdcat_sanitize mdcat.dSYM mdcat_sanitize.dSYM

mdcat: mdcat.c
	$(CC) $(CFLAGS) -o mdcat mdcat.c

sanitize: mdcat.c
	$(CC) $(CFLAGS) $(SANITIZE) -o mdcat_sanitize mdcat.c
