#ifndef COMM_H
#define COMM_H

#include <mpi.h>
#include "../def.h"
#include "../query/query.h"
#include "../query/approximations.h"

#define MASTER_RANK 0

extern PARTITION_ID g_world_size;
extern WORKER_ID g_node_rank;

/* message type */
typedef enum CommMessageType
{
    /* system specific - I/O */
    COMM_SYSTEM_INIT = 0,
    COMM_ACK,
    COMM_FAIL,
    /* data partitioning */
    COMM_DATA_PARTITIONING_BEGIN = 1000,
    COMM_DATA_PARTITIONING_SETUP = COMM_DATA_PARTITIONING_BEGIN,
    COMM_DATA_PARTITIONING_INFO,
    COMM_DATA_PARTITIONING_END,
    /* query specific */
    COMM_QUERY_INIT = 2000,
    COMM_QUERY_BEGIN,
    COMM_QUERY_END,
    /* approximation specific */
    COMM_APPROX_CREATE = 3000,
    COMM_APPROX_DELETE,

    COMM_FIN
}CommMessageTypeE;

typedef enum CommMessageTag
{
    COMM_TAG_EMPTY = 0,
    COMM_TAG_SETUP = 10,
}CommMessageTagE;

/* execution specified operations */
typedef struct UserConfiguration 
{
    // partitioning
    bool partitionFlag = false;
    uint32_t numberOfPartitions = 1000;
    uint32_t batchSize = 1000;
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

/* main listening function */
extern DB_STATUS ListenForMessages();


#endif
