#include <stdio.h>
#include "env/comm_message.h"
#include "env/disk_local_index.h"
#include <bits/getopt_core.h>

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    char c;
    int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &WORLD_SIZE);
	MPI_Comm_rank(MPI_COMM_WORLD, &NODE_RANK);

    // printf("Hi from node %d\n", NODE_RANK);
    
    // std::string argument1(argv[argc-2]);
    // std::string argument2(argv[argc-1]);

    DiskIndexT diskIndex;

    while ((c = getopt(argc, argv, "Pp:jwsraiN:h:c")) != -1)
    {
        switch (c)
        {
            case 'P':
                // perform the partitioning (default 1000 parititons per dimension)
                // printf("Partitioning of data requested\n");
                break;
            case 'p':
                // # of partitions for grid per dimension
                // printf("Will use %d partitions for the partitioning of the data\n", atoi(optarg));
                break;   
            case 'j':
                // intersection join
                break;   
            case 'w':
                // within join
                break;
            case 's':
                // selection/range query
                break;
            case 'r':
                // use refinement
                break;
            /* INTERMEDIATE FILTER */
            case 'a':
                // use APRIL in query
                break;
            case 'i':
                // use RI in query
                break;
            /* APRIL and RI */
            case 'N':
                // set N for APRIL or RI (default N = 16)
                break;
            /* APRIL SPECIFIC */
            case 'h':
                // # of partitions for APRIL
                break;
            case 'c':
                // use compressed APRIL
                break;
            
            default:
                break;
        }
    }
    

    DB_STATUS ret = SystemInit(DISK_SINGLE_MACHINE, &diskIndex);
    if (ret != DB_OK) {       
        MPI_Finalize();
        exit(-1);
    }






    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}
