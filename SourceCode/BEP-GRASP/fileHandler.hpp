/********************************
 * instanceReader.hpp
 *
 * Contains functions to handle file reading
 *
 * */

#ifndef FILEHANDLER_HPP_
#define FILEHANDLER_HPP_

#include <vector>
#include <string>
#include "dataStructures.hpp"
#include "problemInstance.hpp"
#include "solution.hpp"

using namespace std;

class FileHandler{
public:
	int numberOfBuses;
	int busCapacity;
	int numberOfDepots;
	vector<int> busesPerDepot;
	int numberOfAssemblyPoints;
	int totalDemand;
	vector<int> demandPerAssemblyPoint;
	int numberOfShelters;
	int totalShelterCapacity;
	vector<int> capacityPerShelter;
	vector<vector<float>> distanceDepotAP;
	vector<vector<float>> distanceAPShelter;

	FileHandler();
	ProblemInstance * readFile(const string &inputPath);
	void printInput();

	void csvAddHeader();
	void csvAddLine(string inputFilename,time_t timeLimit,int seed, int numberOfIterations, int maxHCIterations, float referenceListPercentage,
			Solution bestSol, float diff);
	void csvAddLineVector(vector<string> inserts);
	void saveSolutionToFile(const std::string& filename, Solution* solution);

	void writeStringVectToFile(const std::string& filename, vector<string> writeVect);
	string removePathFromFilename(string filename);
	string removeExtensionFromFilename(string filename);
};




#endif /* FILEHANDLER_HPP_ */
