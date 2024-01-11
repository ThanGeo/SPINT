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
