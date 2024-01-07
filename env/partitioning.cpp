#include "partitioning.h"

GlobalIndexT g_global_index;
LocalIndexT g_local_index;

DB_STATUS AddPartitionToGlobalIndex(PartitionT* partition){
    if (auto it = g_global_index.activePartitions.find(partition->id); it != g_global_index.activePartitions.end()){
        return ERR_DUPLICATE_PARTITION;
	}
    printf("added partition %d to index, with x = %d and y = %d\n", partition->id, partition->x, partition->y);
    g_global_index.activePartitions[partition->id] = partition;
    return ERR_OK;
}

DB_STATUS GetPartitionFromGlobalIndex(PARTITION_ID &id, PartitionT *partition){
    if (auto it = g_global_index.activePartitions.find(id); it != g_global_index.activePartitions.end()){
		partition = it->second;
        printf("Retrieved partition %d with x = %d and y = %d\n", partition->id, partition->x, partition->y);
        return ERR_OK;
	}
    return ERR_PARTITION_NOT_CREATED;
}

WORKER_ID GetWorkerIDForPartitionID(PARTITION_ID partitionId){
    return partitionId % g_world_size;
}

static void ReadNextPolygonBinary(std::ifstream *fin, SpatialObjectT *polygon) {
    //read polygon id
    fin->read((char*) &polygon->id, sizeof(uint32_t));
    //read vertex count for polygon & reserve space
    fin->read((char*) &polygon->vertexCount, sizeof(uint32_t));
    polygon->vertices.reserve(polygon->vertexCount);
    // for MBR
    polygon->mbr.minP.x = std::numeric_limits<int>::max();
    polygon->mbr.minP.y = std::numeric_limits<int>::max();
    polygon->mbr.maxP.x = -std::numeric_limits<int>::max();
    polygon->mbr.maxP.y = -std::numeric_limits<int>::max();
    //read points polygon
    for(int i=0; i<polygon->vertexCount; i++){
        PointT point;
        //read x, y
        fin->read((char*) &point.x, sizeof(double));
        fin->read((char*) &point.y, sizeof(double));
        //store vertices
        polygon->vertices.emplace_back(point); 
        // keep MBR
        polygon->mbr.minP.x = std::min(point.x, polygon->mbr.minP.x);
        polygon->mbr.minP.y = std::min(point.y, polygon->mbr.minP.y);
        polygon->mbr.maxP.x = std::max(point.x, polygon->mbr.maxP.x);
        polygon->mbr.maxP.y = std::max(point.y, polygon->mbr.maxP.y);
    }
}

static void ClearPolygonBatch(std::vector<SpatialObjectT*> &batch, uint32_t batchSize){
    for (uint32_t i = 0; i<batchSize; i++) {
        delete batch[i];
    }
    batch.clear();
    printf("***** Cleared batch *****\n");
}

PARTITION_ID GetPartitionIDfromPartitionXY(uint32_t x, uint32_t y, uint32_t numberOfPartitions){
    return y * numberOfPartitions + x;
}

PARTITION_ID GetPartitionIDfromPoint(PointT *point, DatasetT *dataset, uint32_t numberOfPartitions){
    uint32_t partitionX = (point->x - dataset->mbr.minP.x) / dataset->spanX;
    uint32_t partitionY = (point->y - dataset->mbr.minP.y) / dataset->spanY;
    return GetPartitionIDfromPartitionXY(partitionX, partitionY, numberOfPartitions);
}

void CreatePartitionFromPoint(PointT *point, DatasetT *dataset, uint32_t numberOfPartitions, PartitionT *partition) {
    partition->x = (point->x - dataset->mbr.minP.x) / dataset->spanX;
    partition->y = (point->y - dataset->mbr.minP.y) / dataset->spanY;
    partition->id = GetPartitionIDfromPartitionXY(partition->x, partition->y, numberOfPartitions);
}

static DB_STATUS AssignPolygonToNodeBatches(DatasetT *dataset, SpatialObjectT* polygon, uint32_t numberOfPartitions,
            std::unordered_map<WORKER_ID, std::vector<SpatialObjectT*>> &batchPerWorker, uint32_t batchSize) {
    printf("----- POLYGON %d ------\n", polygon->id);
    PartitionT* minPartition = new PartitionT;
    PartitionT* maxPartition = new PartitionT;

    // get min and max partitions in grid
    uint32_t minPartitionID = GetPartitionIDfromPoint(&polygon->mbr.minP, dataset, numberOfPartitions);
    uint32_t maxPartitionID = GetPartitionIDfromPoint(&polygon->mbr.maxP, dataset, numberOfPartitions);
    
    // safety checks
    if (minPartitionID < 0 || maxPartitionID < 0) {
        LOG_ERR("Partition id calculated to be less than zero.", ERR_PARTITIONING);
        return ERR_PARTITIONING;
    }
    if (minPartitionID > numberOfPartitions*numberOfPartitions -1 || maxPartitionID > numberOfPartitions*numberOfPartitions - 1) {
        LOG_ERR("Partition id calculated to be larger than g_world_size-1.", ERR_PARTITIONING);
        return ERR_PARTITIONING;
    }
    if (minPartitionID > maxPartitionID) {
        LOG_ERR("Min partion id smaller than max partition id.", ERR_PARTITIONING);
        return ERR_PARTITIONING;
    }
    printf("min and max partition ids: %d and %d\n", minPartitionID, maxPartitionID);

    // check if it is active already, otherwise create it
    DB_STATUS ret = GetPartitionFromGlobalIndex(minPartitionID, minPartition);
    if (ret == ERR_PARTITION_NOT_CREATED) {
        // create the partition and add it to the index
        CreatePartitionFromPoint(&polygon->mbr.minP, dataset, numberOfPartitions, minPartition);
        ret = AddPartitionToGlobalIndex(minPartition);
        if (ret != ERR_OK) {
            LOG_ERR("Adding min partition failed.", ret);
            return ret;
        }
    }
    ret = GetPartitionFromGlobalIndex(maxPartitionID, maxPartition);
    if (ret == ERR_PARTITION_NOT_CREATED) {
        // create the partition and add it to the index
        CreatePartitionFromPoint(&polygon->mbr.maxP, dataset, numberOfPartitions, maxPartition);
        ret = AddPartitionToGlobalIndex(maxPartition);
        if (ret != ERR_OK) {
            LOG_ERR("Adding max partition failed.", ret);
            return ret;
        }
    }

    // flag workers that need to receive
    std::vector<bool> workersToSendPolygonTo;
    for (WORKER_ID i = 0; i<g_world_size; i++) {
        workersToSendPolygonTo.emplace_back(false);
    }

    printf("looping from %d to %d and %d to %d\n", minPartition->x, maxPartition->x, minPartition->y, maxPartition->y);

    // loop all the partitions from min to max and flag the worker nodes that correspond to them
    for(uint32_t i = minPartition->x; i <= maxPartition->x; i++) {
        for(uint32_t j = minPartition->y; j <= maxPartition->y; j++) {
            PARTITION_ID partitionId = GetPartitionIDfromPartitionXY(i, j, numberOfPartitions);
            WORKER_ID workerId = GetWorkerIDForPartitionID(partitionId);
            workersToSendPolygonTo[workerId] = true;
        }
    }

    // add polygon to the batches of the workers that need to receive it
    for (uint32_t i=0; i < g_world_size; i++) {
        if(workersToSendPolygonTo[i]) {
            batchPerWorker[i].emplace_back(polygon);

            // if the batch size exceeds maxbatchsize, send the message
            if (batchPerWorker[i].size() >= batchSize) {
                ret = SendPolygonBatchMessage(batchPerWorker[i], batchPerWorker[i].size());
                if (ret != ERR_OK) {
                    LOG_ERR("Error sending polygon batch message.", ERR_COMM_SEND_MESSAGE);
                    return ERR_COMM_SEND_MESSAGE;
                }
                // // free batch memory and reset
                ClearPolygonBatch(batchPerWorker[i], batchPerWorker[i].size());
            }
        }
    }

    return ERR_OK;
}

DB_STATUS PerformPartitioningBinaryFile(DatasetT *dataset, uint32_t numberOfPartitions, uint32_t maxbatchSize){
    uint32_t lineCounter = 0;
    DB_STATUS ret = ERR_OK;
    
    // open the dataset binary file
    std::ifstream fin(dataset->filePath, std::fstream::in | std::ios_base::binary);
    if (!fin.is_open()){
        LOG_ERR("Error opening file for partitioning.", ERR_OPEN_FILE);
        return ERR_OPEN_FILE;
    }

    //read total polygon count from binary geometry file
	fin.read((char*) &dataset->totalObjects, sizeof(uint32_t));

    // map: worker -> batch
    std::unordered_map<WORKER_ID, std::vector<SpatialObjectT*>> batchPerWorker;
    // initialize batches
    for (WORKER_ID i = 0; i < g_world_size; i++) {
        std::vector<SpatialObjectT*> batch;
        batch.reserve(maxbatchSize);
        batchPerWorker[i] = batch;
    }
    
    // loop polygons
    while (lineCounter < dataset->totalObjects) {
        // init new polygon in batch
        SpatialObjectT *polygonPtr = new SpatialObjectT;
        // read next polygon
        ReadNextPolygonBinary(&fin, polygonPtr);

        // assign to appropriate batch (and send if it is filled)
        ret = AssignPolygonToNodeBatches(dataset, polygonPtr, numberOfPartitions, batchPerWorker, maxbatchSize);
        if (ret != ERR_OK) {
            LOG_ERR("Error assigning polygon to node.", ERR_PARTITIONING);
            return ERR_PARTITIONING;
        }
        printf("Done with object %d / %d\n", lineCounter, dataset->totalObjects);
        lineCounter++;
    }
    fin.close();

    // send the remaining unfulfilled batches
    for(uint32_t i=0; i<g_world_size; i++) {
        if (batchPerWorker[i].size() > 0) {
            ret = SendPolygonBatchMessage(batchPerWorker[i], batchPerWorker[i].size());
            if (ret != ERR_OK) {
                LOG_ERR("Error sending polygon batch message.", ERR_COMM_SEND_MESSAGE);
                return ERR_COMM_SEND_MESSAGE;
            }
            // // free memory and clear
            ClearPolygonBatch(batchPerWorker[i], batchPerWorker[i].size());
        }
    }

    return ret;
}