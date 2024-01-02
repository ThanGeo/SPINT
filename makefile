CC=mpicxx.mpich 
CFLAGS= -g -fopenmp

SOURCES = $(wildcard env/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: mainapp

mainapp: main.o $(OBJECTS)
	$(CC) $(CFLAGS) main.o -o program

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm build/*.o 
	rm program
