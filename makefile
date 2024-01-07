CC=mpicxx.mpich 
CFLAGS= -std=c++17 -O3 -fopenmp

SOURCES =  $(wildcard dataset_info/*.cpp) $(wildcard env/*.cpp) $(wildcard query/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
BUILD = $(wildcard build/*.o)

debug: CFLAGS += -g
debug: all

all: mainapp

mainapp: $(OBJECTS) def.o main.o

link:
	$(CC) $(CFLAGS) $(BUILD) -o program

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm build/*.o
	rm program

