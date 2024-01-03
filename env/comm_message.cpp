#include "comm_message.h"

DB_STATUS SystemInit(DiskIndexTypeE diskType, DiskIndexT *diskIndex){
    // no message needs to be sent for this operation

    if (diskType == DISK_SINGLE_MACHINE) {
        diskIndex->type = DISK_SINGLE_MACHINE;
        // if single machine, master will make the checks
        if (NODE_RANK == MASTER_RANK) {
            return InitializeSingleMachineDiskIndex(diskIndex);
        }
    } else {
        diskIndex->type = DISK_CLUSTER;
        // else, each node has to check their respetive dirs
        return InitializeClusterDiskIndex(NODE_RANK, diskIndex);
    }

    
    return DB_OK;
}