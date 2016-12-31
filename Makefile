EXE=image_edit

build: main image 
	gcc -g -std=c99 -Wall -o $(EXE) obj/main.o obj/image.o

main: src/main.c
	gcc -g -c -std=c99 -Wall -o obj/main.o src/main.c

image: src/image.c
	gcc -g -c -std=c99 -Wall -o obj/image.o src/image.c

run:
	./$(EXE)

clean:
	rm obj/*
	rm $(EXE)
	rm airplane_*
