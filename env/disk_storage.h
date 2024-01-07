#ifndef DISK_INDEX_H
#define DISK_INDEX_H

#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include "../def.h"

typedef enum DiskIndexType {
    DISK_SINGLE_MACHINE,
    DISK_CLUSTER,
}DiskIndexTypeE;

typedef struct DiskIndex
{
    DiskIndexTypeE type;
    std::string mainNodeDataDir = "node_data/";
    std::string spatialDir = mainNodeDataDir + "spatial/";
    std::string approximationDir = mainNodeDataDir + "approximations/";
    std::string aprilDir = approximationDir + "APRIL/";
    std::string riDir = approximationDir + "RI/";
}DiskIndexT;

extern DB_STATUS InitializeSingleMachineDiskIndex(DiskIndexT *diskIndex);

extern DB_STATUS InitializeClusterDiskIndex(uint nodeRank, DiskIndexT *diskIndex);

#endif