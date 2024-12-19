WFLAGS=-Wall -Wextra -pedantic -std=c11 -fsanitize=address -Wno-unused-function -Werror

.PHONY: tests clean

app: src/main.c mcalc4.o
	$(CC) -o mcalc4 src/main.c mcalc4.o $(WFLAGS)

mcalc4.o: src/mcalc4.c
	$(CC) -c src/mcalc4.c $(WFLAGS)

tests: src/tests.c mcalc4.o
	$(CC) -o tests src/tests.c mcalc4.o $(WFLAGS)

clean:
	rm ./*.o ./mcalc4 ./tests
