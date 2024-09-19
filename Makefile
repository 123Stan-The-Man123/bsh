main: ./src/main.c
	gcc -o ./src/main ./src/main.c ./src/built-in.c -lreadline