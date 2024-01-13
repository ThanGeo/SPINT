#ifndef COMM_MESSAGE_H
#define COMM_MESSAGE_H

#include <vector>

#include "comm_def.h"
#include "disk_storage.h"

/* general message struct */

extern DB_STATUS SystemInit(DiskIndexTypeE diskType, DiskIndexT *diskIndex);

extern DB_STATUS SendPolygonBatchMessage(std::vector<SpatialObjectT> &batch, uint32_t batchSize, WORKER_ID workerId);

/* main listening function */
extern DB_STATUS ListenForMessages();

// sends a "ping" message of size one, with the specified message tag
extern DB_STATUS SendPingMessage(CommMessageTagE messageTag, WORKER_ID workerId);

#endif