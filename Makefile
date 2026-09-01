NAME=joyscan
SRC=src
FILTER='MAIN:|DEBUG:'

TEST_SRC=test_src

RAYLIB_SRC=deps/raylib/src
RAYLIB_LIB=$(RAYLIB_SRC)/libraylib.a
CFLAGS=-I$(RAYLIB_SRC)
LDLIBS=$(RAYLIB_LIB) -lm -lpthread -ldl -lrt -lX11

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_SRC) PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC

compile: $(RAYLIB_LIB)
	gcc $(CFLAGS) ./$(SRC)/main.c $(LDLIBS) -o $(NAME)

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
