OBJS = main.c
OBJ_NAME = Minesweeper

all : $(OBJS)
	gcc $(OBJS) -IC:\Development\SDL\include -LC:\Development\SDL2\lib -lmingw32 -lSDL2main -lSDL2 -o $(OBJ_NAME)