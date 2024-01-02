#include <stdio.h>
#include "env/comm_message.h"

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &NODE_RANK);

    // Print off a hello world message
    printf("Hello world from rank %d out of %d\n", NODE_RANK, WORLD_SIZE);

    if(NODE_RANK == 0){
        DB_STATUS ret = BuildInitMessage();
        printf("Master: %d", ret);
    }    

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
