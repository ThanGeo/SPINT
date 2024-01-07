#ifndef COMM_MESSAGE_H
#define COMM_MESSAGE_H

#include <vector>

#include "comm_def.h"
#include "disk_storage.h"

/* general message struct */
typedef struct CommMessage CommMessageT;

DB_STATUS SystemInit(DiskIndexTypeE diskType, DiskIndexT *diskIndex);

DB_STATUS SendPolygonBatchMessage(std::vector<SpatialObjectT*> &batch, uint32_t batchSize);

#endif