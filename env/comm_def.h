#ifndef COMM_H
#define COMM_H

#include <mpi.h>
#include <stdio.h>

extern int32_t WORLD_SIZE;
extern int32_t NODE_RANK;

/* message type */
typedef enum CommMessageType
{
    COMM_INIT,
    COMM_DATA,
    COMM_QUERY,

    COMM_FIN
}CommMessageTypeE;

/* general message struct */
struct CommMessage;




#endif
