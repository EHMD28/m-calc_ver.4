WFLAGS=-lm -Wall -Wextra -pedantic -std=c11 -fsanitize=address -Wno-unused-function -Werror -Wno-unused-command-line-argument
CLI_DIR=src/cli
LIBS_DIR=src/libs
MCALC4_DIR=src/mcalc4
CLI_DIR=src/cli
# CC=gcc
TEST_DIR=tests

.PHONY: tests clean release libs

app: src/main.c mcalc4.o cli.o arachne.o
	$(CC) -o mcalc4 src/main.c mcalc4.o cli.o arachne.o $(WFLAGS)

mcalc4.o: $(MCALC4_DIR)/mcalc4.c
	$(CC) -c $(MCALC4_DIR)/mcalc4.c $(WFLAGS)

cli.o: $(CLI_DIR)/cli.c
	$(CC) -c $(CLI_DIR)/cli.c $(WFLAGS)

arachne.o:
	$(CC) -c $(LIBS_DIR)/arachne-strlib/arachne.c

libs: arachne.o

tests: $(TEST_DIR)/tests.c $(TEST_DIR)/mcalc4_tests.c $(TEST_DIR)/cli_tests.c mcalc4.o arachne.o
	$(CC) -o app-tests $(TEST_DIR)/tests.c\
					$(TEST_DIR)/mcalc4_tests.c\
					$(TEST_DIR)/cli_tests.c\
					mcalc4.o cli.o arachne.o\
					$(WFLAGS)

release: src/main.c
	$(CC) -o mcalc4 src/main.c $(MCALC4_DIR)/mcalc4.c $(CLI_DIR)/cli.c -O3

clean:
	rm ./*.o ./mcalc4 ./tests
