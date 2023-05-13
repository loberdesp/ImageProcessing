
output: funkcje.o
	gcc main.c funkcje.o -o output

funkcje.o: funkcje.c
	gcc funkcje.c -c -lm
