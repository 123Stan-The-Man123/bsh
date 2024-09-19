CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = src/main.c src/built-in.c

main: ./src/main.c
	$(CC) $(CFLAGS) -o ./src/main $(SRC) -lreadline