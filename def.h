#ifndef DEF_H
#define DEF_H

#include <mpi.h>

#define DB_BASE 10000

typedef enum DB_STATUS{
    DB_OK = DB_BASE + 0,
    DB_DIR_ERROR = DB_BASE + 10,

}DB_STATUS;

#endif