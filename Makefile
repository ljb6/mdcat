all: mdcat

clean:
	rm -rf mdcat

mdcat: mdcat.c
	gcc -o mdcat mdcat.c