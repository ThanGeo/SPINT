#include <stdio.h>
#include "env/comm_message.h"
#include "env/disk_local_index.h"

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &NODE_RANK);

    // printf("Hi from node %d\n", NODE_RANK);
    
    std::string pathToFileR(argv[1]);
    std::string pathToFileS(argv[2]);

    DiskIndexT diskIndex;
    

    DB_STATUS ret = SystemInit(DISK_SINGLE_MACHINE, &diskIndex);
    if (ret != DB_OK) {       
        MPI_Finalize();
        exit(-1);
    }






    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
