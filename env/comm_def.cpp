#include "comm_def.h"
#include <vector>

int32_t WORLD_SIZE;
int32_t NODE_RANK;

/* general message struct */
struct CommMessage
{
    CommMessageTypeE type;
    std::vector<void*> data;

};