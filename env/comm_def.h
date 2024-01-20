#ifndef COMM_H
#define COMM_H

#include <mpi.h>
#include "../def.h"
#include "../query/query.h"
#include "../query/approximations.h"
#include "disk_storage.h"

#define MASTER_RANK 0
#define DEFAULT_PARTITIONS_PER_DIM 1000
#define DEFAULT_BATCH_SIZE 10000

extern PARTITION_ID g_world_size;
extern WORKER_ID g_node_rank;


/*
    Only used by master. Stores information about the general topology and data distribution/stats
        - partitions to node mapping
        - objects to partition mapping
*/
typedef struct GlobalIndex
{
    // todo: make a destroy function properly for all necessary
    WORKER_ID owner = MASTER_RANK;
    uint32_t partitionsPerDimension = DEFAULT_PARTITIONS_PER_DIM;
    // active partitions (partitions that have at least one object)
    std::unordered_map<PARTITION_ID, PartitionT*> activePartitions;
    // map: partition id -> node id
    std::unordered_map<PARTITION_ID, WORKER_ID> partitionToNodeMap;
    // map: object id -> partition id
    std::unordered_map<uint32_t, PARTITION_ID> objectToPartitionMap;
} GlobalIndexT;

/*
    Each node keeps one local index (master as well). Stores information about local topology
        - partition to contents mapping
        - participating dataset info (data type, borders, span etc.)
*/
typedef struct LocalIndex
{
    uint32_t partitionsPerDimension = DEFAULT_PARTITIONS_PER_DIM;
    // partition -> its contents (object ID list)
    std::unordered_map<PARTITION_ID, std::vector<uint32_t>> partitionContentsMap;
    uint32_t datasetCount = 0;
    std::vector<DatasetT*> datasets;
} LocalIndexT;

extern GlobalIndexT g_global_index;
extern LocalIndexT g_local_index;
extern DiskIndexT g_disk_index;

/* message type */
typedef enum
{
    /* system specific - I/O */
    COMM_SYSTEM_INIT = 0,
    COMM_ACK,
    COMM_FAIL,
    /* data partitioning */
    COMM_DATA_PARTITIONING_INIT = 1000,
    COMM_DATA_PARTITIONING_SETUP,
    COMM_DATA_PARTITIONING_INFO,
    COMM_DATA_RESET_PARTITION_FILES,
    /* query specific */
    COMM_QUERY_INIT = 2000,
    COMM_QUERY_BEGIN,
    COMM_QUERY_END,
    /* approximation specific */
    COMM_APPROX_CREATE = 3000,
    COMM_APPROX_DELETE,
    /* dataset related */
    COMM_DATASET_INFO = 4000,

    COMM_STOP_LISTENING = 10000,
}CommMessageTagE;

/* execution specified operations */
typedef struct UserConfiguration 
{
    // partitioning
    bool partitionFlag = false;
    uint32_t numberOfPartitions = DEFAULT_PARTITIONS_PER_DIM;
    uint32_t batchSize = DEFAULT_BATCH_SIZE;
    // query
    bool queryFlag;
    QueryTypeE queryType;
    // refinement
    bool refinementFlag = false;
    // approximation
    bool approximationFlag = false;
    ApproxTypeE approximationType;
    /* todo: parameters for approximations */

    // dataset 
    uint32_t datasetCount;  // 2 for joins, 1 otherwise
    std::vector<std::string> datasetPaths;
} UserConfigurationT;



#endif
