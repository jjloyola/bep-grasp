#include <cassert> //"assert.h"
#include "dataStructures.hpp"
#include "fileHandler.hpp"
#include "problemInstance.hpp"
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>


FileHandler::FileHandler()
{
	numberOfBuses = 0;
	busCapacity = 0;
	numberOfDepots =0;
	numberOfAssemblyPoints=0;
	numberOfShelters=0;
	totalShelterCapacity=0;
	totalDemand=0;
}

//Lee el archivo que se entrega al inicio.
ProblemInstance * FileHandler::readFile(const string &inputPath){

	ProblemInstance * instance = new ProblemInstance();
	ifstream file;
	file.open(inputPath.c_str(), ios::in);

	//---get buses info (line 1 of input file)---
	file >> this->numberOfBuses >> this->busCapacity;

	instance->setBusCapacity(this->busCapacity);

	//---get depot info (line 2 of input file)---
	file >> this->numberOfDepots;
	int busesInDepot,busID=0;

	for(int i=0; i<this->numberOfDepots; i++) //for each depot
	{
		file >> busesInDepot;

		Depot * newDepot = new Depot(i,busesInDepot); //create depot objetc

		assert(newDepot);

		//this->busesPerDepot.push_back(busesInDepot);


		for(int j=0;j<busesInDepot;j++) //add buses to depot and to instance
		{
			Bus * newBus = new Bus(busID,this->busCapacity,newDepot);
			assert(newBus);
			newDepot->addBus(newBus);
			instance->addBus(newBus);
			busID++;
		}

		instance->addDepot(newDepot);
	}



	//---get assembly point info (line 3 of input file)---
	file >> this->numberOfAssemblyPoints >> this->totalDemand;
	int demandInAP;
	for(int i=0; i<this->numberOfAssemblyPoints; i++) //for each assembly point
	{
		file >> demandInAP;

		AssemblyPoint * newPoint = new AssemblyPoint(i,demandInAP);

		instance->addAssemblyPoint(newPoint);
		instance->addAssemblyPointByDemand(demandInAP,newPoint);

		//this->demandPerAssemblyPoint.push_back(demandInAP);
	}
	instance->initTotalDemand(this->totalDemand);


	//---get shelter info (line 4 of input file)---
	file >> this->numberOfShelters >> this->totalShelterCapacity;
	int capacity;
	for(int i=0; i<this->numberOfShelters; i++)
	{
		file >> capacity;
		Shelter * newShelter = new Shelter(i,capacity);
		instance->addShelter(newShelter);
		instance->addShelterByCapacity(capacity,newShelter);

		//this->capacityPerShelter.push_back(capacity);
	}
	instance->initTotalShelterCapacity(this->totalShelterCapacity);


	float d;
	//--get depot-AP distances
	int currentDepot;
	for (int i=0; i < this->numberOfDepots; i++)
	{
		file >> currentDepot;
		//vector<float> * distances = new vector<float>;

		for (int j=0; j< this->numberOfAssemblyPoints; j++)
		{
			file >> d;
			//distances->push_back(d);
			instance->getDepot(i)->addDistanceToAP(d);
		}

		//this->distanceDepotAP.push_back(*distances);
	}


	//--get AP-shelter distances
	int currentAP;
	for (int i=0; i < this->numberOfAssemblyPoints; i++)
	{
		file >> currentAP;
		//vector<float> * distances = new vector<float>;
		for (int j=0; j< this->numberOfShelters; j++)
		{
			file >> d;
			//distances->push_back(d);
			instance->getAssemblyPoint(i)->addDistanceToShelter(d);
			instance->getShelter(j)->addDistanceToAP(d);
		}

		//this->distanceDepotAP.push_back(*distances);
	}

	file.close();

	instance->generateInitialTripList();
	instance->generateTripList();

	return instance;
}


void FileHandler::printInput()
{
	//Print buses info
	cout << "Num Buses: " << this->numberOfBuses << endl << "Capacidad buses: " << this->busCapacity << endl;

	cout << endl;

	//Print depots info
	cout << "Num Depots: " << this->numberOfDepots << endl;
	for(int i=0; i<this->numberOfDepots; i++)
	{
		cout << this->busesPerDepot[i] << " buses in depot " << i << endl;
	}

	cout << endl;

	//Print assembly points info
	cout << "Num Assembly points: " << this->numberOfAssemblyPoints << endl;
	for(int i=0; i<this->numberOfAssemblyPoints; i++)
	{
		cout << this->demandPerAssemblyPoint[i] << " persons in AP " << i << endl;
	}

	cout << endl;

	//Print shelter info
	cout << "Num shelters: " << this->numberOfShelters << endl;
	for(int i=0; i<this->numberOfShelters; i++)
	{
		cout << this->capacityPerShelter[i] << " capacity in Shelter " << i << endl;
	}
	cout << endl;


	//print depot-ap distances
	/*for(int i=0; i<this->numberOfDepots; i++)
	{
		cout << "Distance from depot " << i << ":" << endl;
		for(int j=0; j<this->numberOfAssemblyPoints; j++)
		{
			cout << "\tTo AP " << j << ": " << this->distanceDepotAP[i][j] << endl;
		}
	}*/


	return;
}

void FileHandler::csvAddHeader() {
	cout << "TimeLimit;";
	cout << "Seed;";
	//cout << "MaxNumberOfIterations;";
	cout << "ReferenceListPercentage;";
	cout << "MaxHCIterations;";
	cout << "BestFO;";
	cout << "TimeFound;";

	cout << endl;
}

void FileHandler::csvAddLine(string inputFilename,time_t timeLimit,int seed, int numberOfIterations, int maxHCIterations, float referenceListPercentage,
		Solution bestSol, float diff) {
	cout << inputFilename <<";"; //-i
	cout << seed << ";";//-s
	cout << referenceListPercentage << ";";//-ref
	cout << maxHCIterations << ";";//ithc
	cout << timeLimit/(double)CLOCKS_PER_SEC << ";"; //-t
	cout << bestSol.getObjFuncValue() << ";";
	cout << bestSol.timeFound << ";";//time when the best solution was found in seconds
	cout << diff << ";";//difference between cost of largest route bus and shortest route bus
	cout << bestSol.avgTimeToGetOnABus << ";"; //average time to get on a bus
	cout << bestSol.avgTimeToGetOnAShelter << ";"; //average time to get on a shelter
	cout << endl;
}

void FileHandler::csvAddLineVector(vector<string> inserts) {

	for(int i=0;i<inserts.size();i++)
		cout << inserts[i] <<";";

	cout << endl;
}

void FileHandler::writeStringVectToFile(const std::string& filename, vector<string> writeVect)
{
	ofstream file;
	file.open (filename);

	if (file.is_open())
	{
		for (int i=0; i<writeVect.size(); i++)
			file << writeVect.at(i) + "\n";
	}

	file.close();
}

string FileHandler::removePathFromFilename(string filename)
{
	string::size_type i = filename.find_last_of("/\\");
	if (i != std::string::npos)
		filename.erase(0, i+1);

	return filename;

}

string FileHandler::removeExtensionFromFilename(string filename)
{
	string::size_type i = filename.find_last_of(".");
	if (i != std::string::npos)
		filename.erase(i, filename.size()-1);

	return filename;
}


void FileHandler::saveSolutionToFile(const std::string& filename, Solution* solution) {
	std::fstream fs;
	fs.open (filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);

	fs << "------ BEST SOLUTION ------" << endl;
	fs << "Objective Function Value: "<< solution->getObjFuncValue() << "\n\n";

	for (int busID=0; busID<solution->getBusList().size(); busID++)
	{
		Bus currentBus = solution->getBus(busID);
		fs << "Bus " << currentBus.getID() << " - ";
		fs << "Cost " << currentBus.getTotalRouteCost() <<": ";
		//vector<Trip*>
		for(int position=0; position<currentBus.getTripList().size(); position++)
		{
			fs << "(" << currentBus.getTrip(position)->getStartPoint()->getNodeTypeShort() << " " << currentBus.getTrip(position)->getStartPoint()->getID()
				 << "," << currentBus.getTrip(position)->getEndPoint()->getNodeTypeShort() << " " << currentBus.getTrip(position)->getEndPoint()->getID() << ") ";
		}
		fs << endl;
	}
	fs.close();
}
