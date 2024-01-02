CC=mpicxx.mpich 
CFLAGS= -g -fopenmp

SOURCES = $(wildcard env/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
BUILD = $(wildcard build/*.o)

all: mainapp

mainapp: $(OBJECTS) main.o

link:
	$(CC) $(CFLAGS) $(BUILD) -o program

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm build/*.o 
	rm program
