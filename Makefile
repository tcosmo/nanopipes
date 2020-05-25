CC=g++
FLAGS=--std=c++11
LIBS=-lm 
LIBS_SFML=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
FILES=src/*.cpp
INC=$(shell pwd)

main: $(FILES)
	$(CC) -o nanopipes.out $(FILES) $(LIBS) $(LIBS_SFML) -I ${INC} 