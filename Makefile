EXE=image_edit

build: main image 
	gcc -O3 -std=c99 -Wall -o $(EXE) obj/main.o obj/image.o

main: src/main.c
	gcc -O3 -c -std=c99 -Wall -o obj/main.o src/main.c

image: src/image.c
	gcc -O3 -c -std=c99 -Wall -o obj/image.o src/image.c

run:
	./$(EXE)

clean:
	rm obj/*
	rm $(EXE)
