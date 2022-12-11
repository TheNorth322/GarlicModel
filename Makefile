CFLAGS = -lGL -lGLU -lglut 
CC = g++
SOURCE = main.cpp
OBJECTS = $(SOURCE:.c=.o)

all: garlic

garlic: $(OBJECTS)
	$(CC) $(OBJECTS) -o garlic $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf *.o

