WFLAGS=-Wall -Wextra -pedantic -std=c11 -fsanitize=address -Wno-unused-function -Werror
CLI_DIR=src/cli
LIBS_DIR=src/libs
MCALC4_DIR=src/mcalc4

.PHONY: tests clean

app: src/main.c mcalc4.o
	$(CC) -o mcalc4 src/main.c mcalc4.o $(WFLAGS)

mcalc4.o: $(MCALC4_DIR)/mcalc4.c
	$(CC) -c $(MCALC4_DIR)/mcalc4.c $(WFLAGS)

tests: $(MCALC4_DIR)/tests.c mcalc4.o
	$(CC) -o tests $(MCALC4_DIR)/tests.c mcalc4.o $(WFLAGS)

clean:
	rm ./*.o ./mcalc4 ./tests
