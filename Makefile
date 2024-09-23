CC = gcc
CFLAGS = -Wall -Wextra -Werror
SRC = src/main.c src/built-in.c

bin/main: $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^ -lreadline -lbsd
