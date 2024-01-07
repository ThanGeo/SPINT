#ifndef PARTITIONING_H
#define PARTITIONING_H

#include <iostream>
#include <fstream>

#include "../def.h"
#include "comm_def.h"
#include "comm_message.h"

// todo: make a destroy function properly for all necessary
typedef struct GlobalIndex
{
    WORKER_ID owner = MASTER_RANK;
    // active partitions (partitions that have at least one object)
    std::unordered_map<PARTITION_ID, PartitionT*> activePartitions;
    // map: partition id -> node id
    std::unordered_map<PARTITION_ID, WORKER_ID> partitionToNodeMap;
    // map: object id -> partition id
    std::unordered_map<uint32_t, PARTITION_ID> objectToPartitionMap;
    // dataset(s) info (TODO: replace with dataset struct)
    uint32_t datasetCount = 0;
    std::vector<DatasetT*> datasets;
} GlobalIndexT;

// local to each worker node
typedef struct LocalIndex
{
    // partition -> its contents (object ID list)
    std::unordered_map<PARTITION_ID, std::vector<uint32_t>> partitionContentsMap;
} LocalIndexT;

extern inline WORKER_ID GetWorkerIDForPartitionID(PARTITION_ID partitionId);

DB_STATUS GetPartitionFromGlobalIndex(PARTITION_ID &id, PartitionT *partition);

extern DB_STATUS PerformPartitioningBinaryFile(DatasetT *dataset, uint32_t numberOfPartitions, uint32_t batchSize);

extern inline PARTITION_ID GetPartitionIDfromPartitionXY(uint32_t x, uint32_t y, uint32_t numberOfPartitions);

extern inline PARTITION_ID GetPartitionFromPoint(PointT *point, DatasetT *dataset, uint32_t numberOfPartitions);

extern GlobalIndexT g_global_index;
extern LocalIndexT g_local_index;

#endif