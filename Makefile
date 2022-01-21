NAME=joyscan
CFLAGS=-lraylib
SRC=src
FILTER='MAIN:|DEBUG:'

TEST_SRC=test_src


compile:
	gcc $(CFLAGS) ./$(SRC)/main.c -o $(NAME)

run: compile
	stdbuf -oL ./$(NAME)

debug: compile
	stdbuf -oL ./$(NAME) | grep -E $(FILTER)

wejoy:
	sudo modprobe uinput
	wejoy ./wejoy.lua


# test stuff
compile_test:
	gcc ./$(TEST_SRC)/main.c -o test_bin
	# gcc $(CFLAGS) ./$(TEST_SRC)/main.c -o test_bin

run_test: compile_test
	stdbuf -oL ./test_bin
