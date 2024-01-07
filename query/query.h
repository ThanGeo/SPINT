#ifndef QUERY_H
#define QUERY_H

#include "../def.h"

typedef enum QueryType
{
    QT_RANGE = 0,
    QT_JOINS_BEGIN = 10,
    QT_INTERSECTION_J = QT_JOINS_BEGIN,
    QT_WITHIN_J,
    QT_DISTANCE_J,
    QT_JOINS_END,
    QT_KNN = QT_JOINS_END,
} QueryTypeE;

#endif