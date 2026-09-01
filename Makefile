NAME=joyscan
SRC=src
FILTER='MAIN:|DEBUG:'

TEST_SRC=test_src

RAYLIB_SRC=deps/raylib/src
RAYLIB_LIB=$(RAYLIB_SRC)/libraylib.a
CFLAGS=-I$(RAYLIB_SRC)
LDLIBS=$(RAYLIB_LIB) -lm -lpthread -ldl -lrt -lX11

DUALSENSE_SRC=deps/dualsense-multiplatform
DUALSENSE_BUILD=$(DUALSENSE_SRC)/build
DUALSENSE_LIB=$(DUALSENSE_BUILD)/Source/libGamepadCore.a
DUALSENSE_INC=$(DUALSENSE_SRC)/Source/Public

HIDAPI_SRC=deps/hidapi
HIDAPI_INC=$(HIDAPI_SRC)/hidapi

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_SRC) PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC

$(DUALSENSE_LIB):
	cmake -S $(DUALSENSE_SRC) -B $(DUALSENSE_BUILD) -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
	cmake --build $(DUALSENSE_BUILD) --target GamepadCore -j

compile: $(RAYLIB_LIB) $(DUALSENSE_LIB)
	gcc $(CFLAGS) -c $(SRC)/main.c -o main.o
	gcc $(CFLAGS) -I$(HIDAPI_INC) -c $(HIDAPI_SRC)/linux/hid.c -o hid.o
	g++ -std=c++20 -I$(DUALSENSE_INC) -I$(HIDAPI_INC) -c $(SRC)/dualsense/dualsense.cpp -o dualsense.o
	g++ -std=c++20 -I$(DUALSENSE_INC) -I$(HIDAPI_INC) -c $(SRC)/dualsense/linux_hidapi_platform.cpp -o linux_hidapi_platform.o
	g++ main.o hid.o dualsense.o linux_hidapi_platform.o $(DUALSENSE_LIB) $(LDLIBS) -ludev -o $(NAME)

run: compile
	stdbuf -oL ./$(NAME)

debug: compile
	stdbuf -oL ./$(NAME) | grep -E $(FILTER)

# test stuff
compile_test:
	gcc ./$(TEST_SRC)/main.c -o test_bin
	# gcc $(CFLAGS) ./$(TEST_SRC)/main.c -o test_bin

run_test: compile_test
	stdbuf -oL ./test_bin
