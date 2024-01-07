#include "dataset_info.h"

std::unordered_map<std::string, DatasetCodeE> g_filenameToCodeMap = {
	{"T1NA_fixed_binary.dat", TIGER},
	{"T2NA_fixed_binary.dat", TIGER},
	{"T3NA_fixed_binary.dat", TIGER},
	{"O5_Africa_fixed.dat", OSM_AF},
	{"O6_Africa_fixed.dat", OSM_AF},
	{"O5_Asia_fixed.dat", OSM_AS},
	{"O6_Asia_fixed.dat", OSM_AS},
	{"O5_Europe_fixed.dat", OSM_EU},
	{"O6_Europe_fixed.dat", OSM_EU},
	{"O5_NorthAmerica_fixed.dat", OSM_NA},
	{"O6_NorthAmerica_fixed.dat", OSM_NA},
	{"O5_Oceania_fixed.dat", OSM_OC},
	{"O6_Oceania_fixed.dat", OSM_OC},
	{"O5_Southamerica_fixed.dat", OSM_SA},
	{"O6_SouthAmerica_fixed.dat", OSM_SA},
};

//fixed universal coordinates per data set 
static bool SetHardcodedDatasetMBR(DatasetT *dataset){
	switch(dataset->code){
		case TIGER:
			//TIGER DATASET, SET FIXED COORDINATES OF TIGER NORTH AMERICA low48
			// universalMinX = -124.849;
		    // universalMinY = 24.5214;
		    // universalMaxX = -66.8854;
		    // universalMaxY = 49.3844;
			PointT pMin,pMax;
			pMin.x = -124.849;
			pMin.y = 24.5214;
			dataset->mbr.minP = pMin;
			pMax.x = -66.8854;
			pMax.y = 49.3844;
			dataset->mbr.maxP = pMax;
			break;
		case OSM_OC:
			//OCEANIA
			// universalMinX = 112.6;
		    // universalMinY = -51.11;
		    // universalMaxX = 180.5;
		    // universalMaxY = 13.4;
			break;
		case OSM_AS:
			//ASIA
			// universalMinX = 43.9999;
		    // universalMinY = 5.771669;
		    // universalMaxX = 145.36601;
		    // universalMaxY = 82.50001;
			break;
		case OSM_EU:
			//EUROPE
			// universalMinX = -10.594201;
		    // universalMinY = 34.761799;
		    // universalMaxX = 45.893501;
		    // universalMaxY = 71.170701;
			break;
		case OSM_NA:
			//NORTH AMERICA
			// universalMinX = -127.698001;
		    // universalMinY = 12.443799;
		    // universalMaxX = -54.002399;
		    // universalMaxY = 60.686401;
			break;
		case OSM_AF:
			//AFRICA
			// universalMinX = -18.138101;
		    // universalMinY = -34.785201;
		    // universalMaxX = 51.24801;
		    // universalMaxY = 38.339401;
			break;
		case OSM_SA:
			//SOUTH AMERICA
			// universalMinX = -87.361101;
		    // universalMinY = -56.500601;
		    // universalMaxX = -34.78799;
		    // universalMaxY = 12.878401;
			break;
        default:
			// unknown dataset
            return true;
            break;
	}
	// known dataset
    return false;
}

DB_STATUS CalculateDatasetMBR(DatasetT *dataset) {
	// TODO
	LOG_ERR("TODO: Calculate Dataset MBR remains to be implemented.", ERR_FEATURE_TODO);
	return ERR_FEATURE_TODO;





	// return ERR_OK;
}

static void GenerateDatasetNameAndCode(DatasetT *dataset) {
	char dirDelimiter = '/';
	std::stringstream ss(dataset->filePath);
	std::string filename;
	// get filename
	while(std::getline(ss, filename, dirDelimiter))
	{
		// seglist.push_back(segment);
	}
	// after the end of loop it is stored in filename var
	dataset->fileName = filename;

	// get the code if it exists
	if (auto it = g_filenameToCodeMap.find(filename); it != g_filenameToCodeMap.end()){
		dataset->code = it->second;
		return;
	}
	dataset->code = NONE;
}

DB_STATUS SetupDatasetInfo(std::string &pathToDataset, GlobalIndexT &globalIndex){
	DatasetT *dataset = new DatasetT;	//todo: remember to free memory if reusable/resets
	dataset->filePath = pathToDataset;
	// get dataset code
	GenerateDatasetNameAndCode(dataset);
	// get dataset mbr (if any hardcoded)
	bool unknownDatasetFlag = SetHardcodedDatasetMBR(dataset);
	// if yes, return
	if (unknownDatasetFlag) {
		DB_STATUS ret = CalculateDatasetMBR(dataset);
		if (ret != ERR_OK) {
			LOG_ERR("Error calculating dataset MBR.", ret);
		}
	}
	// set dataset spans
	dataset->spanX = dataset->mbr.maxP.x - dataset->mbr.minP.x;
	dataset->spanY = dataset->mbr.maxP.y - dataset->mbr.minP.y;
	// add to global index
	globalIndex.datasetCount += 1;
	globalIndex.datasets.emplace_back(dataset);
	return ERR_OK;
}

