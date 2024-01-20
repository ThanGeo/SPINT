#ifndef DEF_H
#define DEF_H

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

typedef uint32_t WORKER_ID;
typedef uint32_t PARTITION_ID;

/* STATUS AND ERR CODES */
#define DB_BASE 10000
typedef enum DB_STATUS{
    ERR_OK = DB_BASE + 0,
    ERR_FEATURE_TODO = DB_BASE + 1,
    ERR_FEATURE_NOT_SUPPORTED = DB_BASE + 2,
    // config errors
    ERR_QUERY_DATASET_MISMATCH = DB_BASE + 10,
    ERR_DATASET_INFO = DB_BASE + 11,
    // directory I/O errors
    ERR_DIR = DB_BASE + 100,
    ERR_CREATE_FILE =  DB_BASE,
    ERR_OPEN_FILE =  DB_BASE,
    // partitioning errors
    ERR_PARTITIONING = DB_BASE + 200,
    ERR_PARTITION_NOT_CREATED = DB_BASE + 201,
    ERR_DUPLICATE_PARTITION = DB_BASE + 202,
    // communication errors
    ERR_COMM_SEND_MESSAGE = DB_BASE + 300,
    ERR_COMM_RECV_MESSAGE,
    // forbidden action errors
    ERR_FB_BAD_SOURCE = DB_BASE + 400,
    ERR_FB_BAD_DEST,
    // data structure errors
    ERR_KEY_DOESNT_EXIST = DB_BASE + 500,

}DB_STATUS;

/* ERROR LOGGING */
extern void LOG_ERR(std::string error_text, uint32_t error_code);

/* COMMON SPATIAL DATA */
typedef enum SpatialDataType
{
    DTYPE_POINT,
    DTYPE_LINESTRING,
    DTYPE_RECTANGLE,
    DTYPE_POLYGON,
} SpatialDataTypeE;

typedef struct Point
{
    uint32_t id;
    double x,y;
} PointT;

typedef struct Mbr{
    PointT minP, maxP;
} MbrT;

typedef struct SpatialObject
{
    uint32_t id;
    SpatialDataTypeE type;
    uint32_t vertexCount;
    std::vector<PointT> vertices;
    // for objects with extent
    double area;
    MbrT mbr;
} SpatialObjectT;

/* DATASET */

typedef enum{
    // UNKOWN DATASET
    NONE = 0,
    // TIGER
    T1NA = 10,
    T2NA,
    T3NA,
    // OSM per continent
    OSM_AF = 20,
    OSM_AS,
    OSM_EU,
    OSM_NA,
    OSM_OC,
    OSM_SA,
} DatasetCodeE;

typedef struct Dataset
{
    DatasetCodeE code;
    std::string shortCode;
    SpatialDataTypeE dataType;
    std::string filePath;
    std::string fileName;
    std::string partitionFilepath;
    uint32_t totalObjects = 0;
    MbrT mbr;
    double spanX,spanY;
} DatasetT;

// PARTITIONING
typedef struct Partition
{
    PARTITION_ID id;
    uint32_t x,y;

} PartitionT;




// PRINTERS
extern void PrintPolygon(SpatialObjectT *polygon);
extern void PrintDataset(DatasetT *dataset);
#endif