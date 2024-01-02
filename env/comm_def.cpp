#include "comm_def.h"

int32_t WORLD_SIZE;
int32_t NODE_RANK;

/* general message struct */
struct CommMessage
{
    CommMessageTypeE type;
    char* data;

}CommMessageT;