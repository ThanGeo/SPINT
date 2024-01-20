#include "comm_message.h"

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


DB_STATUS SendPingMsg(CommMessageTagE messageTag, WORKER_ID workerId){
    int32_t mpi_ret = MPI_Send(NULL, 0, MPI_CHAR, workerId, messageTag, MPI_COMM_WORLD);
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("MPI_Send termination message failed.", ERR_COMM_SEND_MESSAGE);
        return ERR_COMM_SEND_MESSAGE;
    }
    return ERR_OK;
}

static void FormatPolygonBatchMessages(std::vector<SpatialObjectT> &batch, uint32_t batchSize, 
                std::vector<uint32_t> &setupMessage, std::vector<double> &infoMessage) {
    
    // TAG | DATATYPE | TOTAL POLYGONS |
    setupMessage.emplace_back(COMM_DATA_PARTITIONING_SETUP);
    setupMessage.emplace_back(DTYPE_POLYGON);
    setupMessage.emplace_back(batchSize);
    // TAG | 
    infoMessage.emplace_back(COMM_DATA_PARTITIONING_INFO);

    for(uint32_t i=0; i<batchSize; i++) {
        // id | vertex count | ...
        if (batch[i].id < 0 || batch[i].vertexCount < 0) {
            printf("batch element %d:    polygon %d with vertex count %d\n", i, batch[i].id, batch[i].vertexCount);
        }
        setupMessage.emplace_back(batch[i].id);
        setupMessage.emplace_back(batch[i].vertexCount);
        for (uint32_t j = 0; j<batch[i].vertexCount; j++) {
            // x | y | ...
            infoMessage.emplace_back(batch[i].vertices[j].x);
            infoMessage.emplace_back(batch[i].vertices[j].y);
        }
    }
}

static DB_STATUS ReceiveDatasetInfo(int32_t msgSize, int messageTag){
    DB_STATUS ret = ERR_OK;
    MPI_Status status;
    if (messageTag != COMM_DATASET_INFO) {
        LOG_ERR("Build filepaths message expects the appropriate init flag.", ERR_COMM_RECV_MESSAGE);
        return ERR_COMM_RECV_MESSAGE;
    }
    char *buf = (char*) malloc(msgSize * sizeof(char));
    MPI_Recv(buf, msgSize, MPI_CHAR, MASTER_RANK, messageTag, MPI_COMM_WORLD, &status);
    std::string datasetInfo(buf);

    DatasetT *dataset = new DatasetT;	//todo: remember to free memory if reusable/resets
    /*
        shortCode|filename|partition filepath
    */
    char dirDelimiter = '|';
	std::stringstream ss(datasetInfo);
	std::string token;
	// get message tokens
	std::getline(ss, token, dirDelimiter);
    dataset->shortCode = token;
	std::getline(ss, token, dirDelimiter);
    dataset->fileName = token;
	std::getline(ss, token, dirDelimiter);
    dataset->partitionFilepath = token;

    // get the dataset code from the mapping
    ret = GetDatasetCodeFromShortcode(dataset->shortCode, dataset->code);
    if(ret != ERR_OK) {
        return ret;
    }

    // PrintDataset(dataset);
    g_local_index.datasetCount++;
    g_local_index.datasets.emplace_back(dataset);
    return ret;
}

static DB_STATUS ReceivePingMsg(int32_t msgSize, int messageTag) {
    DB_STATUS ret = ERR_OK;
    MPI_Status status;
    char *buf;
    switch (messageTag) {
        case COMM_DATA_RESET_PARTITION_FILES:
            /* reset partition files */
            buf = (char*) malloc(msgSize * sizeof(char));
            MPI_Recv(buf, msgSize, MPI_CHAR, MASTER_RANK, messageTag, MPI_COMM_WORLD, &status);
            for(uint32_t i=0; i<g_local_index.datasetCount; i++) {
                ret = ResetFile(g_local_index.datasets[i]->partitionFilepath.c_str());
                if (ret != ERR_OK) {
                    return ret;
                }
            }
            break;
    
        default:
            return ERR_FEATURE_TODO;
            break;
    }

    free(buf);
    return ERR_OK;
}

static DB_STATUS ReceivePartitioningInitMsg(int32_t initMsgSize, int messageTag) {
    MPI_Status status;
    if (messageTag != COMM_DATA_PARTITIONING_INIT) {
        LOG_ERR("Partitioning init message expects the appropriate init flag.", ERR_COMM_RECV_MESSAGE);
        return ERR_COMM_RECV_MESSAGE;
    }
    char *buf = (char*) malloc(initMsgSize * sizeof(char));
    MPI_Recv(buf, initMsgSize, MPI_CHAR, MASTER_RANK, messageTag, MPI_COMM_WORLD, &status);
    
    // initialize the file with name in buf
    DB_STATUS ret = InitializeFile(buf);


    free(buf);
    return ret;
}

static DB_STATUS ReceivePartitioningMsgs(int32_t setupMessageElementCount, int messageTag){
    MPI_Status status;
    uint32_t* setupMessage;
    double* infoMessage;
    // receive setup message
    if (messageTag != COMM_DATA_PARTITIONING_SETUP) {
        LOG_ERR("Partitioning expects first message tag to be setup tag", ERR_COMM_RECV_MESSAGE);
        return ERR_COMM_RECV_MESSAGE;
    }
    setupMessage = (uint32_t*) malloc(setupMessageElementCount * sizeof(uint32_t));
	MPI_Recv(setupMessage, setupMessageElementCount, MPI_UINT32_T, MASTER_RANK, COMM_DATA_PARTITIONING_SETUP, MPI_COMM_WORLD, &status);
    // then probe for the info message size that has to follow the setup message
    int32_t mpi_ret = MPI_Probe(0, COMM_DATA_PARTITIONING_INFO, MPI_COMM_WORLD, &status);
    // check tag
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("Partitioning expects second message tag to be info tag. No such message exists.", ERR_COMM_RECV_MESSAGE);
        free(setupMessage);
        return ERR_COMM_RECV_MESSAGE;
    }
    // element count of message
    int32_t infoMessageElementCount;
    MPI_Get_count(&status, MPI_DOUBLE, &infoMessageElementCount);
    infoMessage = (double*) malloc(infoMessageElementCount * sizeof(double));
	MPI_Recv(infoMessage, infoMessageElementCount, MPI_DOUBLE, 0, COMM_DATA_PARTITIONING_INFO, MPI_COMM_WORLD, &status);
    // printf("Node %d succesfully received setup and info messages of sizes %d and %d respectively.\n", g_node_rank, setupMessageElementCount, infoMessageElementCount);
    
    // TODO: remember to free mallocated space after deformatting and saving on disk

    // deformat the messages
    std::vector<SpatialObjectT> batch;
    
    // store on disk 
    SavePartitioningBatch(batch, batch.size());

    // free buffer allocated memory
    free(setupMessage);
    free(infoMessage);
    return ERR_OK;
}

DB_STATUS ListenForMsgs() {
    MPI_Status status;
    DB_STATUS ret = ERR_OK;
    int32_t msgSize;
    // static buffer used for ack/fin messages etc. always of size 1
    char *staticBuffer = (char*) malloc(sizeof(char));
    while(true){
        // since each line differs in size, we must use MPI_Probe() to learn its length
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (status.MPI_TAG) {
            case COMM_DATASET_INFO:
                // filepath related info (file shortcode)
                MPI_Get_count(&status, MPI_CHAR, &msgSize);
                ret = ReceiveDatasetInfo(msgSize, status.MPI_TAG);
                if (ret != ERR_OK) {
                    goto EXIT;
                }
                break;
            case COMM_DATA_RESET_PARTITION_FILES:
                MPI_Get_count(&status, MPI_UINT32_T, &msgSize);
                ret = ReceivePingMsg(msgSize, status.MPI_TAG);
                if (ret != ERR_OK) {
                    goto EXIT;
                }
                break;
            case COMM_DATA_PARTITIONING_INIT:
                // partitioning init (reset partitioned datafiles)
                MPI_Get_count(&status, MPI_CHAR, &msgSize);
                ret = ReceivePartitioningInitMsg(msgSize, status.MPI_TAG);
                if (ret != ERR_OK) {
                    goto EXIT;
                }
                break;
            case COMM_DATA_PARTITIONING_SETUP:
                // partitioning related message (setup/info)
                MPI_Get_count(&status, MPI_UINT32_T, &msgSize);
                ret = ReceivePartitioningMsgs(msgSize, status.MPI_TAG);
                if (ret != ERR_OK) {
                    goto EXIT;
                }
                break;
            
            
            
            
            
            
            case COMM_STOP_LISTENING:
                // partitioning task finished message. Have to receive to remove from queue
                int mpi_ret = MPI_Recv(staticBuffer, 1, MPI_CHAR, 0, COMM_STOP_LISTENING, MPI_COMM_WORLD, &status);
                if (mpi_ret != MPI_SUCCESS) {
                    LOG_ERR("Failed to receive partitioning task finished message.", ERR_COMM_RECV_MESSAGE);
                }
                // break the listening loop
                goto EXIT;
        }
    }
EXIT:
    // remember to free static buffer
    free(staticBuffer);
    return ret;
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
DB_STATUS SendPolygonBatchMsg(std::vector<SpatialObjectT> &batch, uint32_t batchSize, WORKER_ID workerId) {
    std::vector<uint32_t> setupMessageBuf;
    std::vector<double> infoMessageBuf;
    // add the data to the message buffers in proper format
    FormatPolygonBatchMessages(batch, batchSize, setupMessageBuf, infoMessageBuf);
    // printf("Sending batch to node %d:\n", workerId);
    // printf("    - Setup message size: %ld\n",  setupMessageBuf.size());
    // printf("    - Info  message size: %ld\n", infoMessageBuf.size());

    // send setup message
    int mpi_ret = MPI_Send(setupMessageBuf.data(), setupMessageBuf.size(), MPI_UINT32_T, workerId, COMM_DATA_PARTITIONING_SETUP, MPI_COMM_WORLD);
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("MPI_Send partitioning setup message failed.", ERR_COMM_SEND_MESSAGE);
        return ERR_COMM_SEND_MESSAGE;
    }
    // send info message
    mpi_ret = MPI_Send(infoMessageBuf.data(), infoMessageBuf.size(), MPI_DOUBLE, workerId, COMM_DATA_PARTITIONING_INFO, MPI_COMM_WORLD);
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("MPI_Send partitioning info message failed.", ERR_COMM_SEND_MESSAGE);
        return ERR_COMM_SEND_MESSAGE;
    }

    // printf("Sent message to node %d\n", workerId);
    return ERR_OK;
}

DB_STATUS SendStringMsgWithTagToAll(std::string &stringMsg, CommMessageTagE messageTag) {
    // sanity check
    if (g_node_rank != MASTER_RANK) {
        LOG_ERR("Error, only the master can send string message to all.", ERR_FB_BAD_SOURCE);
        return ERR_FB_BAD_SOURCE;
    }
    // send to all (except self)
    for(uint32_t i=0; i<g_world_size; i++) {
        if (i != MASTER_RANK) {
            int mpi_ret = MPI_Send(stringMsg.data(), stringMsg.size() + 1, MPI_CHAR, i, messageTag, MPI_COMM_WORLD);
            if (mpi_ret != MPI_SUCCESS) {
                LOG_ERR("MPI Send failed for string vector.", ERR_COMM_SEND_MESSAGE);
                return ERR_COMM_SEND_MESSAGE;
            }
        }
    }
    return ERR_OK;
}

DB_STATUS SendStringMsgWithTagToWorker(std::string &stringMsg, WORKER_ID workerId, CommMessageTagE messageTag) {
    // sanity check
    if (g_node_rank != MASTER_RANK) {
        LOG_ERR("Error, only the master can send string message to all.", ERR_FB_BAD_SOURCE);
        return ERR_FB_BAD_SOURCE;
    }
    if (workerId == MASTER_RANK) {
        LOG_ERR("Error, master can't send message to self.", ERR_FB_BAD_DEST);
        return ERR_FB_BAD_DEST;
    }
    // send to worker
    int mpi_ret = MPI_Send(stringMsg.data(), stringMsg.size() + 1, MPI_CHAR, workerId, messageTag, MPI_COMM_WORLD);
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("MPI Send failed for string vector.", ERR_COMM_SEND_MESSAGE);
        return ERR_COMM_SEND_MESSAGE;
    }
    return ERR_OK;
}

DB_STATUS SendPartitionFileResetMsgToAll(){
    // sanity check
    if (g_node_rank != MASTER_RANK) {
        LOG_ERR("Error, only the master can send partition file reset message to all.", ERR_FB_BAD_SOURCE);
        return ERR_FB_BAD_SOURCE;
    }
    // send to all (except self)
    for(uint32_t i=0; i<g_world_size; i++) {
        if (i != MASTER_RANK) {
            DB_STATUS ret = SendPingMsg(COMM_DATA_RESET_PARTITION_FILES, i);
            if (ret != ERR_OK) {
                return ret;
            }
        }
    }
    return ERR_OK;
}


