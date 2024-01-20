#ifndef PARTITIONING_H
#define PARTITIONING_H

#include <iostream>
#include <fstream>

#include "../def.h"
#include "comm_def.h"
#include "comm_message.h"

extern inline WORKER_ID GetWorkerIDForPartitionID(PARTITION_ID partitionId);

extern DB_STATUS PerformPartitioningBinaryFile(DatasetT *dataset, uint32_t batchSize);

extern inline PARTITION_ID GetPartitionIDfromPartitionXY(uint32_t x, uint32_t y, uint32_t numberOfPartitions);

#endif