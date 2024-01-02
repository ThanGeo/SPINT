CC=mpicxx.mpich
CFLAGS=-g -fopenmp


all: mainapp

mainapp: main.o
	$(CC) $(CFLAGS) main.o -o program

%.o: %.cpp
	$(CC) $(CFLAGS) -c $^

clean:
	rm *.o program