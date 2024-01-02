#include <stdio.h>
#include "def.h"
#include "env/comm_def.h"

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &NODE_RANK);

    // Print off a hello world message
    printf("Hello world from rank %d out of %d\n", NODE_RANK, WORLD_SIZE);

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
