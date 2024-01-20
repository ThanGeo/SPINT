#include <string>

#include "disk_storage.h"
#include "comm_def.h"   //todo: remove when transitioning to cluster dev

DB_STATUS CheckDirectory(std::string dirPath){
    DIR* nodeDataDir = opendir(dirPath.c_str());
    if(nodeDataDir) {
        /* Directory exist, all is fine. */
        closedir(nodeDataDir);
    }else if(ENOENT == errno) {
        /* Directory does not exist. Create it */
        int ret = mkdir(dirPath.c_str(), 0777);
        if (ret != 0) {
            printf("Error %d: unknown mkdir error.\n", ERR_DIR);
            return ERR_DIR;
        }
    }else{
        /* opendir() failed for some other reason. */
        printf("Error %d: unknown opendir error.\n", ERR_DIR);
        return ERR_DIR;
    }

    return ERR_OK;
}  

DB_STATUS InitializeSingleMachineDiskIndex(DiskIndexT *diskIndex){
    // check main directory
    DB_STATUS ret = CheckDirectory(diskIndex->mainNodeDataDir);
    if(ret != ERR_OK){
        return ret;
    }

    // check spatial data directory
    ret = CheckDirectory(diskIndex->spatialDir);
    if(ret != ERR_OK){
        return ret;
    }

    // check approximation main directory
    ret = CheckDirectory(diskIndex->approximationDir);
    if(ret != ERR_OK){
        return ret;
    }
    // april directory
    ret = CheckDirectory(diskIndex->aprilDir);
    if(ret != ERR_OK){
        return ret;
    }
    // ri directory
    ret = CheckDirectory(diskIndex->riDir);
    if(ret != ERR_OK){
        return ret;
    }

    return ERR_OK;
}

DB_STATUS ResetFile(const char* filePath) {
    printf("Node %d reseting file %s\n", g_node_rank, filePath);
    std::ofstream fout(filePath, std::fstream::out | std::ios_base::binary);
    if(!fout.is_open()){
        LOG_ERR("Error creating file on disk.", ERR_CREATE_FILE);
        return ERR_CREATE_FILE;
    }
    fout.close();
    return ERR_OK;

}    
DB_STATUS InitializeFile(const char* filePath) {
    // if it exists, leave it be
    std::ifstream fin(filePath, std::fstream::in | std::ios_base::binary);
    if (fin.is_open()){
        fin.close();
        return ERR_OK;
    }
    // else, create empty file 
    std::ofstream fout(filePath, std::fstream::out | std::ios_base::binary);
    if(!fout.is_open()){
        LOG_ERR("Error creating file on disk.", ERR_CREATE_FILE);
        return ERR_CREATE_FILE;
    }
    fout.close();
    return ERR_OK;
}

DB_STATUS InitializeClusterDiskIndex(uint nodeRank, DiskIndexT *diskIndex){
    /* --- REMOVE THIS? ---*/
    // todo: temp, change for cluster or take as input, because we might need to save on SSD or smth
    std::string clusterPathToNodeData = "node_data/cluster/node_" + std::to_string(nodeRank) + "_data/";
    std::string spatialDir = clusterPathToNodeData + "spatial/";

    std::string approximationDir = clusterPathToNodeData + "approximations/";
    std::string aprilDir = approximationDir + "APRIL/";
    std::string riDir = approximationDir + "RI/";

    diskIndex->mainNodeDataDir = clusterPathToNodeData;
    diskIndex->spatialDir = spatialDir;
    diskIndex->approximationDir = approximationDir;
    diskIndex->aprilDir = aprilDir;
    diskIndex->riDir = riDir;

    /*   ---   */

    // check main directory
    DB_STATUS ret = CheckDirectory(diskIndex->mainNodeDataDir);
    if(ret != ERR_OK){
        return ret;
    }

    // check spatial data directory
    ret = CheckDirectory(diskIndex->spatialDir);
    if(ret != ERR_OK){
        return ret;
    }

    // check approximation main directory
    ret = CheckDirectory(diskIndex->approximationDir);
    if(ret != ERR_OK){
        return ret;
    }
    // april directory
    ret = CheckDirectory(diskIndex->aprilDir);
    if(ret != ERR_OK){
        return ret;
    }
    // ri directory
    ret = CheckDirectory(diskIndex->riDir);
    if(ret != ERR_OK){
        return ret;
    }

    return ERR_OK;
}


 DB_STATUS SavePartitioningBatch(std::vector<SpatialObjectT> &batch, uint32_t batchSize) {
    
    

    return ERR_OK;
 }

DB_STATUS PartitionFileReset() {
    // initialize master's partition files
    for(uint32_t i=0; i<g_local_index.datasetCount; i++) {
        DB_STATUS ret = ResetFile(g_local_index.datasets[i]->partitionFilepath.c_str());
        if (ret != ERR_OK) {
            return ret;
        }
    }
    return ERR_OK;
}
