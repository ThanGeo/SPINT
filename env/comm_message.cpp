#include "comm_message.h"

/* general message struct */
struct CommMessage
{
    CommMessageTypeE type;
    // std::vector<void*> data;

};

DB_STATUS SystemInit(DiskIndexTypeE diskType, DiskIndexT *diskIndex){
    // no message needs to be sent for this operation
    // TODO: maybe in cluster it needs to send a message with the info 
    if (diskType == DISK_SINGLE_MACHINE) {
        diskIndex->type = DISK_SINGLE_MACHINE;
        // if single machine, master will make the checks
        if (g_node_rank == MASTER_RANK) {
            return InitializeSingleMachineDiskIndex(diskIndex);
        }
    } else {
        diskIndex->type = DISK_CLUSTER;
        // else, each node has to check their respetive dirs
        return InitializeClusterDiskIndex(g_node_rank, diskIndex);
    }
    return ERR_OK;
}

static void FormatPolygonBatchMessage(std::vector<SpatialObjectT*> &batch, uint32_t batchSize, 
                std::vector<uint32_t> &setupMessage, std::vector<double> &infoMessage) {
    
    // TAG | DATATYPE | TOTAL POLYGONS |
    setupMessage.emplace_back(COMM_DATA_PARTITIONING_SETUP);
    setupMessage.emplace_back(DTYPE_POLYGON);
    setupMessage.emplace_back(batchSize);
    // TAG | 
    infoMessage.emplace_back(COMM_DATA_PARTITIONING_INFO);

    for(uint32_t i=0; i<batchSize; i++) {
        // id | vertex count | ...
        setupMessage.emplace_back(batch[i]->id);
        setupMessage.emplace_back(batch[i]->vertexCount);
        for (uint32_t j = 0; j<batchSize; j++) {
            // x | y | ...
            infoMessage.emplace_back(batch[i]->vertices[j].x);
            infoMessage.emplace_back(batch[i]->vertices[j].y);
        }
    }
}

/*
* Polygon Batch Message consists of 2 individual messages:
*   - setup message (uint32_t): instruct the workers to setup their buffers of what they are about to receive
*           TAG | DATATYPE | TOTAL POLYGONS | ID_0 | VERTEX_COUNT_0 | ID_1 | ... | 
*
*   - info message (double): dountains the values of the points x,y
*           TAG | x | y | x | y | ... |
*
*/
DB_STATUS SendPolygonBatchMessage(std::vector<SpatialObjectT*> &batch, uint32_t batchSize) {
    std::vector<uint32_t> setupMessage;
    std::vector<double> infoMessage;
    
    // add the data to the messages in proper format
    FormatPolygonBatchMessage(batch, batchSize, setupMessage, infoMessage);

    // send
    // TODO make the send
    return ERR_FEATURE_TODO;
    return ERR_OK;
}