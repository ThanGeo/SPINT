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


DB_STATUS SendPingMessage(CommMessageTagE messageTag, WORKER_ID workerId){
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

static DB_STATUS ReceivePartitioningMessages(int32_t setupMessageElementCount, int messageTag){
    MPI_Status status;
    uint32_t* setupMessage;
    double* infoMessage;
    // receive setup message
    if (messageTag != COMM_DATA_PARTITIONING_SETUP) {
        LOG_ERR("Partitioning expects first message tag to be setup tag", ERR_COMM_RECV_MESSAGE);
        return ERR_COMM_RECV_MESSAGE;
    }
    setupMessage = (uint32_t*) malloc(setupMessageElementCount * sizeof(uint32_t));
	MPI_Recv(setupMessage, setupMessageElementCount, MPI_UINT32_T, 0, COMM_DATA_PARTITIONING_SETUP, MPI_COMM_WORLD, &status);
    // then probe for the info message size that has to follow the setup message
    int32_t mpi_ret = MPI_Probe(0, COMM_DATA_PARTITIONING_INFO, MPI_COMM_WORLD, &status);
    // check tag
    if (mpi_ret != MPI_SUCCESS) {
        LOG_ERR("Partitioning expects second message tag to be info tag. No such message exists.", ERR_COMM_RECV_MESSAGE);
        return ERR_COMM_RECV_MESSAGE;
    }
    // element count of message
    int32_t infoMessageElementCount;
    MPI_Get_count(&status, MPI_DOUBLE, &infoMessageElementCount);
    infoMessage = (double*) malloc(infoMessageElementCount * sizeof(double));
	MPI_Recv(infoMessage, infoMessageElementCount, MPI_DOUBLE, 0, COMM_DATA_PARTITIONING_INFO, MPI_COMM_WORLD, &status);
    // printf("Node %d succesfully received setup and info messages of sizes %d and %d respectively.\n", g_node_rank, setupMessageElementCount, infoMessageElementCount);
    
    // TODO: remember to free mallocated space after deformatting and saving on disk

    SavePartitioningBatch();

    free(setupMessage);
    free(infoMessage);
    return ERR_OK;
}

DB_STATUS ListenForMessages() {
    MPI_Status status;
    DB_STATUS ret = ERR_OK;
    // static buffer used for ack/fin messages etc. always of size 1
    char *staticBuffer = (char*) malloc(sizeof(char));
    while(true){
        // since each line differs in size, we must use MPI_Probe() to learn its length
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == COMM_DATA_PARTITIONING_SETUP) {
            // partitioning related message (setup/info)
            int32_t setupMessageCount;
            MPI_Get_count(&status, MPI_UINT32_T, &setupMessageCount);
            ret = ReceivePartitioningMessages(setupMessageCount, status.MPI_TAG);
            if (ret != ERR_OK) {
                return ret;
            }
        } else if (status.MPI_TAG == COMM_STOP_LISTENING) {
            // partitioning task finished message. Have to receive to remove from queue
	        int mpi_ret = MPI_Recv(staticBuffer, 1, MPI_CHAR, 0, COMM_STOP_LISTENING, MPI_COMM_WORLD, &status);
            if (mpi_ret != MPI_SUCCESS) {
                LOG_ERR("Failed to receive partitioning task finished message.", ERR_COMM_RECV_MESSAGE);
            }
            // break the listening loop
            break;
        }
    }
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
DB_STATUS SendPolygonBatchMessage(std::vector<SpatialObjectT> &batch, uint32_t batchSize, WORKER_ID workerId) {
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
