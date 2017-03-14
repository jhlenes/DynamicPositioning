CC = gcc
CFLAGS = -g -Wall -I/usr/X11R6/include
FILES = *.c
LIBS = -lphidget21 -lpthread -lglut -lGLU -lGL
OUT_EXE = DynamicPositioning

build: $(FILES)
	$(CC) $(CFLAGS) -o $(OUT_EXE) $(FILES) $(LIBS)

clean:
	rm $(OUT_EXE)

rebuild: clean build

run:
	./$(OUT_EXE)
