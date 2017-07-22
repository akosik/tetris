OBJS = main.cpp shader.cpp text_renderer.cpp texture_renderer.cpp texture.cpp playfield_renderer.cpp playfield.cpp tetromino.cpp tetromino_renderer.cpp game.cpp hud_renderer.cpp

CC = g++

INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include -I/usr/local/include/freetype2 

LIBRARY_PATHS = -L/usr/local/libs -I/opt/x11/lib

COMPILER_FLAGS = -w -std=c++14

LINKER_FLAGS = -framework OpenGL -lglfw -lglew -lfreetype

DEBUG_FLAGS = -g

OBJ_NAME = main

all: $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug:
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm $(OBJ_NAME)
