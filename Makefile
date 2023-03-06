CC = clang
EXE = bin/struo
SRC = src/main.c
LIBS = lib/glad/src/glad.o lib/glfw/src/libglfw3.a lib/cglm/libcglm.a -lm
FLAGS = -g3 -std=c99 -Wall -Wextra -Wno-unused-parameter
INCLUDE = -Ilib/glad/include -Ilib/glfw/include -Ilib/cglm/include -Ilib/stb

.PHONY: build run libs

build:
	mkdir -p bin
	$(CC) -o $(EXE) $(FLAGS) $(INCLUDE) $(SRC) $(LIBS)

run: build
	$(EXE)

libs:
	cd lib/glad && clang -o src/glad.o -Iinclude -c src/glad.c
	cd lib/cglm && cmake -DCGLM_STATIC=ON && make
	cd lib/glfw && cmake . && make
