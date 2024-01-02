#ifndef COMM_H
#define COMM_H

#include <mpi.h>
#include <stdio.h>

int32_t WORLD_SIZE;
int32_t NODE_RANK;

/* message type */
typedef enum CommMessageType
{
    COMM_INIT,
    COMM_DATA,
    COMM_QUERY,

    COMM_FIN
}CommMessageTypeE;

/* general message struct */
struct CommMessage
{
    CommMessageTypeE type;
    char* data;

}CommMessageT;




#endif
