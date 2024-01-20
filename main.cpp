#include <stdio.h>
#include <bits/getopt_core.h>

#include "def.h"
#include "env/comm_message.h"
#include "env/disk_storage.h"
#include "env/partitioning.h"
#include "query/query.h"
#include "query/approximations.h"
#include "dataset_info/dataset_info.h"

DB_STATUS PerformConfiguredOptions(UserConfigurationT *config){
    DB_STATUS ret = ERR_OK;
    // first get the dataset related info (borders, names etc...)
    for (uint32_t i = 0; i<config->datasetCount; i++) {
        ret = SetupDatasetInfo(config->datasetPaths[i]);
        if (ret != ERR_OK) {
            LOG_ERR("Error setting up dataset info.", ERR_DATASET_INFO);
            return ret;
        }
    }
    
    
    // Partition
    if (config->partitionFlag) {
        // TODO: IF SPECIFIED, MASTER HAS TO SEND THE VALUE TO THE WORKERS!!! MAYBE INCLUDE IN SYSTEM INIT?
        g_global_index.partitionsPerDimension = config->numberOfPartitions;
        g_local_index.partitionsPerDimension = config->numberOfPartitions;
        
        // since re-partitioning has been selected, the nodes must reset their respective partitioned data files
        ret = SendPartitionFileResetMsgToAll();
        if (ret != ERR_OK) {
            LOG_ERR("Error. Initializing partitioning files failed.", ERR_PARTITIONING);
            return ret;
        }
        // reset local file
        ret = PartitionFileReset();
        if (ret != ERR_OK) {
            return ret;
        }

        // perform the partitioning
        for (uint32_t i=0; i<g_local_index.datasetCount; i++) {
            // printf("Partitioning dataset %s\n", g_global_index.datasets[i]->fileName.c_str());
            ret = PerformPartitioningBinaryFile(g_local_index.datasets[i], config->batchSize);
            if (ret != ERR_OK) {
                LOG_ERR("Error. Partitioning failed.", ERR_PARTITIONING);
                return ret;
            }
        }
    }



    // tasks finished, send termination messsage
    for (uint32_t i=0; i<g_world_size; i++) {
        if (i != MASTER_RANK) {
            // printf("Master sending termination ping to node %d\n.", i);
            // then send partitioning termination message
            ret = SendPingMsg(COMM_STOP_LISTENING, i);
            if (ret != ERR_OK) {
                LOG_ERR("Error sending stop listening message.", ERR_COMM_SEND_MESSAGE);
                return ERR_COMM_SEND_MESSAGE;
            }
        }
    }

    return ret;
}

DB_STATUS VerifyConfiguration(UserConfigurationT *config, std::string *argument1, std::string *argument2) {
    if (config->queryFlag) {
        if (*argument1 == "") {
            LOG_ERR("Error. Didn't specify dataset(s) for query.", ERR_QUERY_DATASET_MISMATCH);
            return ERR_QUERY_DATASET_MISMATCH;
        }
        config->datasetCount = 1;
        config->datasetPaths.emplace_back(*argument1);
        if (config->queryType >= QT_JOINS_BEGIN || config->queryType < QT_JOINS_END) {
            // if its a join, it needs 2 datasets
            if (*argument2 == "") {
                LOG_ERR("Error. Didn't specify second dataset for join query.", ERR_QUERY_DATASET_MISMATCH);
                return ERR_QUERY_DATASET_MISMATCH;
            }
            config->datasetCount = 2;
            config->datasetPaths.emplace_back(*argument2);
        }

    }
    return ERR_OK;
}

DB_STATUS ParseConfiguration(int argc, char **argv, UserConfigurationT *config) {
    char c;
    // the master handles the user parameters/arguments
    while ((c = getopt(argc, argv, "Pp:b:jwsraiN:h:c")) != -1)
    {
        switch (c)
        {
            case 'P':
                // perform the partitioning (default 1000 parititons per dimension)
                // printf("Partitioning of data requested\n");
                config->partitionFlag = true;
                break;
            case 'p':
                // # of partitions for grid per dimension
                // printf("Will use %d partitions for the partitioning of the data\n", atoi(optarg));
                config->numberOfPartitions = atoi(optarg);
                break; 
            case 'b':
                // # of objects per batch/message (for partitioning, default 10000)
                config->batchSize = atoi(optarg);
                break;
            case 'j':
                // intersection join
                config->queryFlag = true;
                config->queryType = QT_INTERSECTION_J;
                break;   
            case 'w':
                // within join
                config->queryFlag = true;
                config->queryType = QT_WITHIN_J;
                break;
            case 's':
                // selection/range query
                config->queryFlag = true;
                config->queryType = QT_RANGE;
                break;
            case 'r':
                // use refinement
                config->refinementFlag = true;
                break;
            /* INTERMEDIATE FILTER */
            case 'a':
                // use APRIL in query
                config->approximationFlag = true;
                config->approximationType = POL_APRIL;
                break;
            case 'i':
                // use RI in query
                config->approximationFlag = true;
                config->approximationType = POL_RI;
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
    
    /* verify conditions: TODO double check validity */
    // 2 datasets if join, 1 if other query
    // last 2 (or 1) arguments have to be the datasets
    std::string argument1(argv[argc-2]);
    std::string argument2(argv[argc-1]);
    // printf("%s\n", argument1.c_str());
    // printf("%s\n", argument2.c_str());
    return VerifyConfiguration(config, &argument1, &argument2);
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    char c;
    int provided;
    int32_t rank, wsize;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	// MPI_Init_thread(&mpi_argc, &mpi_argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &wsize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    g_world_size = wsize;
    g_node_rank = rank;

    // struct for the user configuration for this execution
    UserConfigurationT config;

    /* FOR DEBUGGING */
    // printf("Hi from node %d\n", g_node_rank);
    // if (g_node_rank == MASTER_RANK)
    // {
    //     int move;
    //     std::cout << "Press enter to continue: ";
    //     std::cin >> move;
    // }
    // MPI_Barrier(MPI_COMM_WORLD);
    /* ************ */

    // Initialize system
    DB_STATUS ret = SystemInit(DISK_SINGLE_MACHINE, &g_disk_index);
    if (ret != ERR_OK) {
        LOG_ERR("Failed when initializing the system.", ret);       
        goto EXIT_SAFELY;
    }

    if (g_node_rank == MASTER_RANK) {
        // parse and verify the configuration
        DB_STATUS ret = ParseConfiguration(argc, argv, &config);
        if (ret != ERR_OK) {
            LOG_ERR("Failed when parsing the configuration.", ret);   
            goto EXIT_SAFELY;
        }
        // perform the configured options in order
        ret = PerformConfiguredOptions(&config);
        if (ret != ERR_OK) {
            LOG_ERR("Failed when performing the configured options.", ret);       
            goto EXIT_SAFELY;
        }
    } else {
        // Workers go straight to listening/waiting for messages
        DB_STATUS ret = ListenForMsgs();
        if (ret != ERR_OK) {
            LOG_ERR("Failed when listening for messages.", ERR_COMM_RECV_MESSAGE);
            goto EXIT_SAFELY;
        }
    }

    



EXIT_SAFELY:
    // Barrier before termination
    MPI_Barrier(MPI_COMM_WORLD);

    // Finalize the MPI environment.
    MPI_Finalize();
    printf("Node %d successfully terminated\n", g_node_rank);

    return 0;
}
