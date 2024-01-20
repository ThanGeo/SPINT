#ifndef DISK_INDEX_H
#define DISK_INDEX_H

#include <iostream>
#include <fstream>
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
    // directories
    std::string mainNodeDataDir = "node_data/";
    std::string spatialDir = mainNodeDataDir + "spatial/";
    std::string approximationDir = mainNodeDataDir + "approximations/";
    std::string aprilDir = approximationDir + "APRIL/";
    std::string riDir = approximationDir + "RI/";
    // files
    std::unordered_map<DatasetCodeE, DatasetT*> datasetMap;  // key = dataset code, value = partitioned dataset filepath
}DiskIndexT;

extern DB_STATUS InitializeSingleMachineDiskIndex(DiskIndexT *diskIndex);

extern DB_STATUS InitializeClusterDiskIndex(uint nodeRank, DiskIndexT *diskIndex);

extern DB_STATUS SavePartitioningBatch(std::vector<SpatialObjectT> &batch, uint32_t batchSize);

extern DB_STATUS PartitionFileReset();

extern DB_STATUS ResetFile(const char* filePath);
extern DB_STATUS InitializeFile(const char* fileName);

#endif