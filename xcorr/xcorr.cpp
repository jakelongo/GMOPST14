// xcorr.cpp : cross-correlation given dwdbfile
//

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Macro for checking if allocation has taken place
// If any malloc fails then close everything down and let the OS pickup the trash!
#define CHECK_ALLOC(_funcName, _var, _varName) \
  if (NULL == _var) { \
    std::cout << _funcName << ":" << _varName << " Malloc returned NULL pointer." << std::endl; \
    return 0; \
      } \

// Function to write trace data out to a file
uint8_t writeTrace(double* traceData, uint32_t traceLength, std::string tracePath) {
	// File object
	std::ofstream traceFile;

	// Open file at given path
	traceFile.open(tracePath.c_str(), std::ifstream::out | std::ifstream::binary);

	// Check the file has openened, return othwerwise
	if (!traceFile.is_open()) {
		std::cout << "Unable to open file " << tracePath << "for output." << std::endl;
		return 0;
	}

	// Iterate through the data pointer and write out the trace
	for (uint32_t pointIndex = 0; pointIndex < traceLength; ++pointIndex) {
		traceFile.write(reinterpret_cast<char*> (&(traceData[pointIndex])), sizeof(double));
	}

	// Housekeeping
	traceFile.close();

	// Return with SUCCESS
	return 1;

}

uint8_t readTraceData(double*** traceData, uint32_t dataWidth,  std::string traceListFilePath, uint32_t* traceCount, uint32_t traceStartIndex, uint32_t traceLength) {
	// File handles
	std::ifstream traceListFile;
	std::ifstream traceDataFile;

	// Strings to check and store trace paths
	std::string				 traceDataFilePath;
	std::vector<std::string> traceDataFilePaths;
	
	// Check for valid trace files with trace length
	uint32_t traceFileLength;
	uint32_t iteratorCounter;

	// Pointer to create a flat memory space for traces
	double* traceDataFlat;
	void* tempDataStore;

	// Open the list file and go straight to the end
	traceListFile.open(traceListFilePath.c_str(), std::ifstream::in | std::ifstream::binary);

	// Check we've managed to open the file
	if (!traceListFile.is_open()) {
		std::cout << "Unable to open trace list file." << std::endl;
		return 0;
	}

	// Check each trace file is of a valid length and ignore those which are not
	traceFileLength = traceStartIndex + traceLength;
	while (std::getline(traceListFile, traceDataFilePath)) {
		traceDataFile.open(traceDataFilePath.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
		// Check the file has been found and openend.
		if (traceDataFile.is_open()) {
			if (traceDataFile.tellg() >= traceFileLength) {
				traceDataFilePaths.push_back(traceDataFilePath);
			}
			traceDataFile.close();
		}
	}

	// Done with the list file for now;
	traceListFile.close();
	*traceCount = traceDataFilePaths.size();

	// Allocate the space for all the traces
	traceDataFlat = (double*)malloc(traceDataFilePaths.size() * traceLength * sizeof(double));
	CHECK_ALLOC("readTraceData", traceDataFlat, "traceDataFlat");

	*traceData = (double**)malloc(traceLength * sizeof(double*));
	CHECK_ALLOC("readTraceData", *traceData, "*traceData");

	// Map the flat memory structure to a 2-d array of pointers
	for (uint32_t i = 0; i < traceLength; ++i) {
		(*traceData)[i] = &(traceDataFlat[traceDataFilePaths.size() * i]);
	}

	// Go through all the trace files and copy the data into memory
	tempDataStore = malloc(traceLength * dataWidth);
	CHECK_ALLOC("readTraceData", tempDataStore, "tempDataStore");

	iteratorCounter = 0;
	for (std::vector<std::string>::const_iterator traceFileIterator = traceDataFilePaths.begin(); \
		traceFileIterator != traceDataFilePaths.end(); ++traceFileIterator) {

		// Open the trace file and move to the start index
		traceDataFile.open((*traceFileIterator).c_str(), std::ifstream::in | std::ifstream::binary);
		traceDataFile.seekg(traceStartIndex *dataWidth);

		// Read in data from the trace file and temporarily store in array
		switch (dataWidth) {
		case 1:
			traceDataFile.read(reinterpret_cast<char*>((uint8_t*)tempDataStore), (traceLength * dataWidth));
			// Transpose the data as move to matrix store
			for (uint32_t i = 0; i < traceLength; ++i) {
				(*traceData)[i][iteratorCounter] = ((uint8_t*)tempDataStore)[i];
			}
			break;
		case 2:
			traceDataFile.read(reinterpret_cast<char*>((uint16_t*)tempDataStore), (traceLength * dataWidth));
			// Transpose the data as move to matrix store
			for (uint32_t i = 0; i < traceLength; ++i) {
				(*traceData)[i][iteratorCounter] = ((uint16_t*)tempDataStore)[i];
			}
			break;
		case 8:
			traceDataFile.read(reinterpret_cast<char*>((double*)tempDataStore), (traceLength * dataWidth));
			// Transpose the data as move to matrix store
			for (uint32_t i = 0; i < traceLength; ++i) {
				(*traceData)[i][iteratorCounter] = ((double*)tempDataStore)[i];
			}
			break;
		default:
			traceDataFile.read(reinterpret_cast<char*>((uint8_t*)tempDataStore), (traceLength * dataWidth));
			// Transpose the data as move to matrix store
			for (uint32_t i = 0; i < traceLength; ++i) {
				(*traceData)[i][iteratorCounter] = ((uint8_t*)tempDataStore)[i];
			}
			break;
		}

	
		// close the trace file and move to next file
		traceDataFile.close();
		++iteratorCounter;
	}

	// Housekeeping
	free(tempDataStore);
	traceDataFilePaths.clear();

	return 1;
}

// Cross-correlation calculation on give trace matrix
uint8_t xcorr(double** traceData, uint32_t traceLen, uint32_t traceCount, uint32_t windowSize, std::string outputPath) {

	// Container pointers for calculating results
	double* sumTrace;
	double* sum2Trace;
	double* stdDevTrace;
	double* xcorrTrace;

	// Allocate memory and move forward
	sumTrace	= (double*)calloc(traceLen, sizeof(double));
	sum2Trace	= (double*)calloc(traceLen, sizeof(double));
	stdDevTrace = (double*)calloc(traceLen, sizeof(double));

	// Check all the allocations have succeeded
	CHECK_ALLOC("xcorr", sumTrace, "sumTrace");
	CHECK_ALLOC("xcorr", sum2Trace, "sum2Trace");
	CHECK_ALLOC("xcorr", stdDevTrace, "stdDevTrace");

	// Go through trace matrix and calculate the intermediate values for Pearsons correlation coefficient
	for (uint32_t dataCounter = 0; dataCounter < traceLen; ++dataCounter) {
		for (uint32_t traceCounter = 0; traceCounter < traceCount; ++traceCounter) {
			sumTrace[dataCounter] += traceData[dataCounter][traceCounter];
			sum2Trace[dataCounter] += traceData[dataCounter][traceCounter] * traceData[dataCounter][traceCounter];
		}

		stdDevTrace[dataCounter] = sqrt((traceCount * sum2Trace[dataCounter]) - (sumTrace[dataCounter] * sumTrace[dataCounter]));
	}

	// Allocate memory for cross-correlation result
	xcorrTrace = (double*)calloc(traceLen, sizeof(double));
	CHECK_ALLOC("xcorr", xcorrTrace, "xcorrTrace")

	// Go through each point and calculate the cross-correlation with respect
	// to all previous points (- windowSize)
	for (int32_t indexA = 0; indexA < (int32_t)traceLen; ++indexA) {
		double xcorrPoint;
		double scratch;
		std::cout << "\rTreating point: " << std::setfill('0') << std::setw(8) << indexA;
		xcorrPoint = 0.0;
		for (int32_t indexB = 0; indexB < (indexA - (int32_t)windowSize); ++indexB) {
			scratch = 0.0;
			for (uint32_t traceCounter = 0; traceCounter < traceCount; ++traceCounter) {
				scratch += (traceData[indexA][traceCounter] * traceData[indexB][traceCounter]);
			}
			scratch *= traceCount;
			scratch = (scratch - (sumTrace[indexA] * sumTrace[indexB])) / (stdDevTrace[indexA] * stdDevTrace[indexB]);
			if (scratch > xcorrPoint) {
				xcorrPoint = scratch;
			}
		}
		xcorrTrace[indexA] = xcorrPoint;
	}
	std::cout << std::endl;

	// Output our data and some intermediate values for good measure
	writeTrace(xcorrTrace,	traceLen, outputPath + std::string("/_crossCorrelation.dwfm"));
	writeTrace(sumTrace,	traceLen, outputPath + std::string("/_intermediateSum.dwfm"));
	writeTrace(sum2Trace,	traceLen, outputPath + std::string("/_intermediatesum2.dwfm"));
	writeTrace(stdDevTrace, traceLen, outputPath + std::string("/_intermediateStdDev.dwfm"));

	// Return with SUCCESS
	return 1;
}


int main(int argc, char* argv[])
{
	double** traceData = NULL;
	
	if (argc < 5) {
		std::cout << "Usage: ./xcorr [data-width] [traceFileListPath.txt] [outputDirectory] [traceStartPoint] [traceStopPoint] [windowSize]" << std::endl;
		std::cout << "[data-width] can be 1,2 or 8 (for uint8, uint16 or double)" << std::endl;
		return 0;
	}

	uint32_t    dataWidth     = atoi(argv[1]);
	std::string traceListPath = argv[2];
	std::string outputPath    = argv[3];
	uint32_t	traceStart	  = atoi(argv[4]);
	uint32_t	traceStop	  = atoi(argv[5]);
	uint32_t	windowSize	  = atoi(argv[6]);
	uint32_t	traceCount	  = 0;

	std::cout << "Reading trace list and pulling in traces...";

	if (!(readTraceData(&traceData, dataWidth, traceListPath, &traceCount, traceStart, (traceStop - traceStart)))) {
		return 0;
	}

	std::cout << "Done" << std::endl;

	std::cout << "Read calling cross-correlation function..." << std::endl;

	if (!(xcorr(traceData, (traceStop - traceStart), traceCount, windowSize, outputPath))) {
		return 0;
	}

	std::cout << "Done..." << std::endl;

	free(traceData);

	return 1;
}

