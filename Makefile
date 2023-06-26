all: build remove

build:
	gcc amoebic_game.c -o amoebic_game -lncurses
	./amoebic_game

remove:
	rm -rf ./amoebic_game