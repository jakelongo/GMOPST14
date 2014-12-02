// s_c.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#include "stdafx.h"
#include <direct.h>
#define mkdir(_path, _mode) _mkdir(_path)
#elif __linux
#include <sys/stat.h>
#include <sys/types.h>
#elif __APPLE__
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <stdint.h>
#include <stdlib.h>

uint8_t concatTraces(uint32_t dataWidth, std::string traceListFilePath_A, std::string traceListFilePath_B, std::string outputPrefix, uint32_t concatPoint) {
	// File handles
	std::ifstream traceListFile_A;
	std::ifstream traceListFile_B;
	std::ofstream traceListFile_C;

	std::ifstream traceDataFile_A;
	std::ifstream traceDataFile_B;
	std::ofstream traceDataFile_C;

	std::string traceDataPath_A;
	std::string traceDataPath_B;
	std::string traceDataPath_C;

	// Check for valid trace files with trace length
	uint32_t fileCounter = 0;
	uint32_t fileSize = 0;

	void* tempData_A;
	void* tempData_B;
	char pathString[1024];

	// Format output strings
	std::string traceListFilePath_C = std::string("DATA/") + outputPrefix + std::string("_traceList.txt");
	std::string traceDataPathRoot_C = std::string("DATA/") + outputPrefix;

	// Open the list file and go straight to the end
	traceListFile_A.open(traceListFilePath_A.c_str(), std::ifstream::in | std::ifstream::binary);
	traceListFile_B.open(traceListFilePath_B.c_str(), std::ifstream::in | std::ifstream::binary);
	traceListFile_C.open(traceListFilePath_C.c_str(), std::ifstream::out | std::ifstream::binary);

	// Check all the files were opened successfully
	if (!traceListFile_A.is_open()) {
		std::cout << "Unable to open " << traceListFilePath_A << std::endl;
		return 0;
	}

	if (!traceListFile_B.is_open()) {
		std::cout << "Unable to open " << traceListFilePath_B << std::endl;
		return 0;
	}

	if (!traceListFile_C.is_open()) {
		std::cout << "Unable to open " << traceListFilePath_C << std::endl;
		return 0; 
	}

	// Make root directory for trace files
	mkdir("DATA", 0777);
	mkdir(traceDataPathRoot_C.c_str(), 0777);

	tempData_A = malloc(concatPoint * dataWidth);

	// Assume a 1-to-1 relationship with list file entries
	while (std::getline(traceListFile_A, traceDataPath_A) && std::getline(traceListFile_B, traceDataPath_B)) {

		// Get trace files
		traceDataFile_A.open(traceDataPath_A.c_str(), std::ifstream::in | std::ifstream::binary);
		traceDataFile_B.open(traceDataPath_B.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);

		if (!(traceDataFile_A.is_open() && traceDataFile_B.is_open())){
			std::cout << "Unable to open a trace file at " << traceDataPath_A << " or " << traceDataPath_B << std::endl;
			return 0;
		}

		// Check filesize
		fileSize = traceDataFile_B.tellg();
		traceDataFile_B.seekg(std::ifstream::beg);
		tempData_B = malloc((fileSize - (concatPoint * dataWidth)));

		// Make directory if necessary
		if ((fileCounter % 10000) == 0) {
			sprintf(pathString, "%s/d%03d", traceDataPathRoot_C.c_str(), (fileCounter / 10000));
			mkdir(pathString, 0777);
		}

		// Set path for output file
		sprintf(pathString, "%s/d%03d/%s_%04d.dwfm", traceDataPathRoot_C.c_str(), (fileCounter / 10000), outputPrefix.c_str(), (fileCounter % 10000));

		// Open and check file for output
		traceDataFile_C.open(pathString, std::ifstream::out | std::ifstream::binary);
		if (!(traceDataFile_C.is_open())){
			std::cout << "Unable to open a trace file at " << std::string(pathString) << std::endl;
			return 0;
		}

		// Copy part A
		traceDataFile_A.read((char*)tempData_A, (concatPoint*dataWidth));
		traceDataFile_C.write((char*)tempData_A, (concatPoint*dataWidth));

		// Copy part B
		traceDataFile_B.seekg(concatPoint*dataWidth);
		traceDataFile_B.read((char*)tempData_B, (fileSize - (concatPoint*dataWidth)));
		traceDataFile_C.write((char*)tempData_B, (fileSize - (concatPoint*dataWidth)));

		// Close all the files
		traceDataFile_A.close();
		traceDataFile_B.close();
		traceDataFile_C.close();

		// free temp data
		free(tempData_B);

		// Write to log file
		traceListFile_C << std::string(pathString) << std::endl;

		// Next file
		fileCounter++;
	}

	// Housekeeping
	free(tempData_A);
	traceListFile_A.close();
	traceListFile_B.close();
	traceListFile_C.close();

	return 1;
}


int main(int argc, char* argv[])
{

	if (argc < 4) {
		std::cout << "Usage: ./s_c [data-width] [traceFileListPath_A.txt] [traceFileListPath_B.txt] [outputPrefix] [concatPoint]" << std::endl;
		std::cout << "[data-width] can be 1,2 or 8 (for uint8, uint16 or double)" << std::endl;
		return 0;
	}

	uint32_t dataWidth				= atoi(argv[1]);
	std::string traceFileListPath_A = argv[2];
	std::string traceFileListPath_B = argv[3];
	std::string outputPrefix        = argv[4];
	uint32_t concatPoint			= atoi(argv[5]);

	concatTraces(dataWidth, traceFileListPath_A, traceFileListPath_B, outputPrefix, concatPoint);

	return 1;
}

;