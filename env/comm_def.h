#ifndef COMM_H
#define COMM_H

#include <mpi.h>
#include <stdio.h>
#include "../def.h"

#define MASTER_RANK 0

extern int32_t WORLD_SIZE;
extern int32_t NODE_RANK;

/* message type */
typedef enum CommMessageType
{
    /* system specific - I/O */
    COMM_SYSTEM_INIT = 0,
    COMM_ACK,
    COMM_FAIL,
    /* data partitioning */
    COMM_DATA_PARTITIONING = 1000,
    /* query specific */
    COMM_QUERY_INIT = 2000,
    COMM_QUERY_BEGIN,
    COMM_QUERY_END,
    /* approximation specific */
    COMM_APPROX_CREATE = 3000,
    COMM_APPROX_DELETE,

    COMM_FIN
}CommMessageTypeE;

typedef enum CommMessageTag
{
    COMM_TAG_EMPTY = 0,
    COMM_TAG_SETUP = 10,
}CommMessageTagE;

/* general message struct */
typedef struct CommMessage CommMessageT;




#endif
