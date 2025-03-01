WFLAGS=-lm -Wall -Wextra -pedantic -std=c11 -fsanitize=address -Wno-unused-function -Werror
CLI_DIR=src/cli
LIBS_DIR=src/libs
MCALC4_DIR=src/mcalc4
CLI_DIR=src/cli
# CC=gcc

.PHONY: tests clean release

app: src/main.c mcalc4.o cli.o
	$(CC) -o mcalc4 src/main.c mcalc4.o cli.o $(WFLAGS)

mcalc4.o: $(MCALC4_DIR)/mcalc4.c
	$(CC) -c $(MCALC4_DIR)/mcalc4.c $(WFLAGS)

cli.o: $(CLI_DIR)/cli.c
	$(CC) -c $(CLI_DIR)/cli.c $(WFLAGS)

tests: $(MCALC4_DIR)/tests.c mcalc4.o
	$(CC) -o tests $(MCALC4_DIR)/tests.c mcalc4.o $(WFLAGS)

release: src/main.c
	$(CC) -o mcalc4 src/main.c $(MCALC4_DIR)/mcalc4.c $(CLI_DIR)/cli.c -O3

clean:
	rm ./*.o ./mcalc4 ./tests
