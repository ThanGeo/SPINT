#include "def.h"

/* ERROR LOGGING */
void LOG_ERR(std::string error_text, uint32_t error_code) {
    error_text = "*** " + error_text;
    error_text += " code: %d\n";
    fprintf(stderr, error_text.c_str(), error_code);
}

void PrintPolygon(SpatialObjectT *polygon)
{
    printf("Polygon %d:\n", polygon->id);
    printf("(%f,%f)", polygon->vertices[0].x, polygon->vertices[0].y);
    for(uint32_t i =1;i<polygon->vertexCount; i++) {
        printf(",(%f,%f)", polygon->vertices[i].x, polygon->vertices[i].y);
    }
    printf("\n");
    printf("MBR: (%f,%f),(%f,%f)\n", polygon->mbr.minP.x, polygon->mbr.minP.y, polygon->mbr.maxP.x, polygon->mbr.maxP.y);
}

void PrintDataset(DatasetT *dataset)
{
    printf("Dataset with code %d: \n", dataset->code);
    printf("    shortcode: %s \n", dataset->shortCode.c_str());
    printf("    filename: %s \n", dataset->fileName.c_str());
    printf("    filepath: %s \n", dataset->filePath.c_str());
    printf("    partition filepath: %s \n", dataset->partitionFilepath.c_str());
    printf("    datatype: %d\n", dataset->dataType);
}