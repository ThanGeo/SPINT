#ifndef COMM_MESSAGE_H
#define COMM_MESSAGE_H

#include <vector>

#include "comm_def.h"
#include "disk_storage.h"
#include "../dataset_info/dataset_info.h"

/* general message struct */

extern DB_STATUS SystemInit(DiskIndexTypeE diskType, DiskIndexT *diskIndex);

extern DB_STATUS SendPolygonBatchMsg(std::vector<SpatialObjectT> &batch, uint32_t batchSize, WORKER_ID workerId);

/* main listening function */
extern DB_STATUS ListenForMsgs();

// sends a "ping" message of size one, with the specified message tag
extern DB_STATUS SendPingMsg(CommMessageTagE messageTag, WORKER_ID workerId);

extern DB_STATUS SendStringMsgWithTagToAll(std::string &stringMsg, CommMessageTagE messageTag);
extern DB_STATUS SendStringMsgWithTagToWorker(std::string &stringMsg, WORKER_ID workerId, CommMessageTagE messageTag);

extern DB_STATUS SendPartitionFileResetMsgToAll();

#endif