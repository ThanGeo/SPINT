#include "dataset_info.h"

std::unordered_map<std::string, DatasetCodeE> g_filenameToCodeMap = {
	{"T1NA_fixed_binary.dat", T1NA},
	{"T2NA_fixed_binary.dat", T2NA},
	{"T3NA_fixed_binary.dat", T3NA},
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

std::unordered_map<std::string, std::string> g_shortCodeToFileName = {
	{"T1NA","T1NA_fixed_binary.dat"},
	{"T2NA","T2NA_fixed_binary.dat"},
	{"T3NA","T3NA_fixed_binary.dat"},
	{"O5AF","O5_Africa_fixed.dat"},
	{"O6AF","O6_Africa_fixed.dat"},
	{"O5AS","O5_Asia_fixed.dat"},
	{"O6AS","O6_Asia_fixed.dat"},
	{"O5EU","O5_Europe_fixed.dat"},
	{"O6EU","O6_Europe_fixed.dat"},
	{"O5NA","O5_NorthAmerica_fixed.dat"},
	{"O6NA","O6_NorthAmerica_fixed.dat"},
	{"O5OC","O5_Oceania_fixed.dat"},
	{"O6OC","O6_Oceania_fixed.dat"},
	{"O5SA","O5_Southamerica_fixed.dat"},
	{"O6SA","O6_SouthAmerica_fixed.dat"},
};

std::unordered_map<std::string, std::string> g_fileNameToShortCode = {
	{"T1NA_fixed_binary.dat","T1NA"},
	{"T2NA_fixed_binary.dat","T2NA"},
	{"T3NA_fixed_binary.dat","T3NA"},
	{"O5_Africa_fixed.dat","O5AF"},
	{"O6_Africa_fixed.dat","O6AF"},
	{"O5_Asia_fixed.dat","O5AS"},
	{"O6_Asia_fixed.dat","O6AS"},
	{"O5_Europe_fixed.dat","O5EU"},
	{"O6_Europe_fixed.dat","O6EU"},
	{"O5_NorthAmerica_fixed.dat","O5NA"},
	{"O6_NorthAmerica_fixed.dat","O6NA"},
	{"O5_Oceania_fixed.dat","O5OC",},
	{"O6_Oceania_fixed.dat","O6OC"},
	{"O5_Southamerica_fixed.dat","O5SA"},
	{"O6_SouthAmerica_fixed.dat","O6SA",},
};

//fixed universal coordinates per data set 
static bool SetHardcodedDatasetMBR(DatasetT *dataset){
	switch(dataset->code){
		case T1NA:
		case T2NA:
		case T3NA:
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

DB_STATUS GetDatasetCodeFromShortcode(std::string shortCode, DatasetCodeE &datasetCode){
	std::string fileName;
	if (auto it = g_shortCodeToFileName.find(shortCode); it != g_shortCodeToFileName.end()){
		fileName = it->second;
	}else{
		LOG_ERR("shortcode doenst exist in g_shortCodeToFileName", ERR_KEY_DOESNT_EXIST);
		return ERR_KEY_DOESNT_EXIST;
	}
	if (auto it = g_filenameToCodeMap.find(fileName); it != g_filenameToCodeMap.end()){
		datasetCode = it->second;
	}else {
		LOG_ERR("filename doenst exist in g_filenameToCodeMap", ERR_KEY_DOESNT_EXIST);
		return ERR_KEY_DOESNT_EXIST;
	}
	return ERR_OK;
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
	while(std::getline(ss, filename, dirDelimiter));
	// after the end of loop it is stored in filename var
	dataset->fileName = filename;
	printf("filename: %s\n", filename.c_str());

	// keep the file name short code
	ss.str("");
	ss.clear();
	ss.str(filename);
	std::string token;
	std::getline(ss, token, '_');
	dataset->shortCode = token;

	// get the code if it exists
	if (auto it = g_filenameToCodeMap.find(filename); it != g_filenameToCodeMap.end()){
		dataset->code = it->second;
		return;
	}
	dataset->code = NONE;
}

std::string BuildPartitionFilepathsSingleMachine(std::string &shortcode, WORKER_ID forWorkerId) {
	// generate the partitioned file name and path
    std::string partitionedFilename(shortcode);
	std::string partitionedFilePath;
    partitionedFilename += "_" + std::to_string(forWorkerId) + "_partitioned_" + std::to_string(g_global_index.partitionsPerDimension) + ".dat";
	partitionedFilePath = g_disk_index.mainNodeDataDir + partitionedFilename;
	// DB_STATUS ret = InitializeFile(partitionedFilePath.c_str());
	// if (ret != ERR_OK) {
	// 	LOG_ERR("Failed to initialize partitioned data file.", ERR_CREATE_FILE);
	// 	return ERR_COMM_SEND_MESSAGE;
	// }
	// set partitioned file path mapping  TODO
	// g_disk_index.partitionedDataFilepathsMap[dataset->code] = partitionedFilePath;

	return partitionedFilePath;
}

std::string BuildPartitionFilepathsCluster(std::string &shortcode) {
	return "ERR_FEATURE_TODO";
}

/* send the dataset name shortcode to the workers to build their paths */
static DB_STATUS DistributeDatasetInfo(DatasetT *dataset){
	DB_STATUS ret = ERR_OK;
	/*  Message format: concatenated strings delimited by '|'*/
	/*
	 *	shortCode|filename|partition filepath
	*/
	std::string partitionFilePath;
	for(uint32_t i=0; i<g_world_size; i++) {
		// first, set partition data filepaths
		if (g_disk_index.type == DISK_SINGLE_MACHINE) {
			partitionFilePath = BuildPartitionFilepathsSingleMachine(dataset->shortCode, i);
		}else {
			partitionFilePath = BuildPartitionFilepathsCluster(dataset->shortCode);
		}
		// send
		if (i != MASTER_RANK) {
			std::string message = dataset->shortCode + "|" + dataset->fileName + "|" + partitionFilePath;
			// send shortCode to worker
			ret = SendStringMsgWithTagToWorker(message, i, COMM_DATASET_INFO);
			if (ret != ERR_OK) {
				LOG_ERR("Failed to send partitioned filepath msg.", ERR_COMM_SEND_MESSAGE);
				return ERR_COMM_SEND_MESSAGE;
			}
		}
	}
	// save local path 
	dataset->partitionFilepath = BuildPartitionFilepathsSingleMachine(dataset->shortCode, MASTER_RANK);
	return ret;
}

DB_STATUS SetupDatasetInfo(std::string &pathToDataset){
	DB_STATUS ret = ERR_OK;
	DatasetT *dataset = new DatasetT;	//todo: remember to free memory if reusable/resets
	dataset->filePath = pathToDataset;
	// get dataset code
	GenerateDatasetNameAndCode(dataset);
	// get dataset mbr (if any hardcoded)
	bool unknownDatasetFlag = SetHardcodedDatasetMBR(dataset);
	// if yes, return
	if (unknownDatasetFlag) {
		ret = CalculateDatasetMBR(dataset);
		if (ret != ERR_OK) {
			LOG_ERR("Error calculating dataset MBR.", ret);
		}
	}
	// set dataset spans
	dataset->spanX = dataset->mbr.maxP.x - dataset->mbr.minP.x;
	dataset->spanY = dataset->mbr.maxP.y - dataset->mbr.minP.y;
	// add to global index
	ret = DistributeDatasetInfo(dataset);
	if (ret != ERR_OK) {
		LOG_ERR("Error sending dataset shortcode.", ret);
	}

	// store to local index and disk index
	g_local_index.datasetCount += 1;
	g_local_index.datasets.emplace_back(dataset);

	g_disk_index.datasetMap[dataset->code] = dataset;
	PrintDataset(dataset);

	return ERR_OK;
}

