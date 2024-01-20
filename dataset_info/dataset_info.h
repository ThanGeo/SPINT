#ifndef DATASET_H
#define DATASET_H

#include "../def.h"
#include "../env/partitioning.h"

// when calculating the borders of a dataset, allow a margin so that the objects do not "touch" the borders
//      to avoid rounding errors
#define MARGIN_PERCENTAGE 0.001

extern DB_STATUS SetupDatasetInfo(std::string &pathToDataset);

extern DB_STATUS GetDatasetCodeFromShortcode(std::string shortCod, DatasetCodeE &datasetCode);

#endif