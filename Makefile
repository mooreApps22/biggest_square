all:
	cc -Wall -Wextra -Werror main.c -o bsq
clean:
	rm files/file*
	rm bsq
