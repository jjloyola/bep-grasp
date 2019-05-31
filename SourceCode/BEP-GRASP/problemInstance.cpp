#include "problemInstance.hpp"
#include "dataStructures.hpp"
#include "tripMapping.hpp"
#include <iostream>

using namespace std;

void ProblemInstance::addShelterByCapacity(int capacity_, Shelter* shelter_) {
	this->sheltersByCapacity.insert(
			std::pair<int, Shelter*>(capacity_, shelter_));
}

/*void ProblemInstance::printInstance(bool printBusList, bool printDepotList, bool printAssemblyPointList,
		bool printShelterList, bool showDistanceMatrix,bool showNeighborMultimap, bool printInitialTripList)
{
	if(printBusList) this->printBusList();
	if(printDepotList) this->printDepotList(showDistanceMatrix);
	if(printAssemblyPointList) this->printAssemblyPointList(showDistanceMatrix);
	if(printShelterList) this->printShelterList(showDistanceMatrix);
	if(printInitialTripList) this->printInitialTripList();

	return;
}*/


void ProblemInstance::printBusList()
{
	cout << "\n---------- Buses ----------" << endl;
	for(Bus * current: this->busList)
	{
		current->print();
		cout << "----" << endl;
	}
}

void ProblemInstance::printSolution()
{
	cout << "------SOLUTION------" << endl;
	for (int i=0; i<this->busList.size(); i++)
	{
		cout << "Bus " << busList.at(i)->getID() << ": ";
		busList.at(i)->printTripList();
	}
}

void ProblemInstance::printBusPositions()
{
	cout << "\n----------Bus positions-------"<<endl;
	for(Bus * current: this->busList)
	{
		cout << "\n->Bus " << current->getID() << ": ";
		cout << "Node type " << current->getCurrentLocation()->getNodeType();
		cout << " ID " << current->getCurrentLocation()->getID() << endl;
	}
	cout << "--------------------------" << endl;
}

void ProblemInstance::printDepotList(bool showDistanceMatrix,bool showNeighborMultimap){
	cout << "\n---------- Depots ----------" << endl;
	for(Depot * current: this->depotList)
	{
		current->print(showDistanceMatrix,showNeighborMultimap);
		cout << "----" << endl;
	}
}

void ProblemInstance::printAssemblyPointList(bool showDistanceMatrix,bool showNeighborMultimap){
	cout << "\n---------- Assembly Points ----------" << endl;
	for(AssemblyPoint * current: this->assemblyPointList)
	{
		current->print(showDistanceMatrix,showNeighborMultimap);
		cout << "----" << endl;
	}
}

void ProblemInstance::printShelterList(bool showDistanceMatrix,bool showNeighborMultimap){
	cout << "\n---------- Shelters ----------" << endl;
	for(Shelter * current: this->shelterList)
	{
		current->print(showDistanceMatrix,showNeighborMultimap);
		cout << "----" << endl;
	}
}


//Generate trip list from Depots to Assembly Points
void ProblemInstance::generateInitialTripList()
{
	int totalDepots = this->depotList.size();
	int totalAP = this->assemblyPointList.size();

	/*create lexicographic ordering
	 *
	 * Trip 	Depot	Assembly Point
	 * 	0		 0			0
	 * 	1		 0			1
	 * 	2		 0			2
	 * 	3		 1			0
	 * 	4		 1			1
	 * 	5		 1			2
	 * 	...		...			...
	 * */
	int totalTrips = totalDepots * totalAP;
	int tripID=0;
	for (int depotID=0; depotID<totalDepots; depotID++)
	{
		for (int apID=0; apID<totalAP; apID++)
		{
			//send pointer to depot and assembly point to constructor
			Trip * newTrip = new Trip(tripID, this->depotList[depotID],this->assemblyPointList[apID],true);

			//add tour to initial tour list
			this->initialTripList.push_back(newTrip);

			this->depotList[depotID]->addNeighbor(newTrip->getDistance(),newTrip->getEndPoint());
			this->depotList[depotID]->addTrip(newTrip);
			tripID++;
		}

		//make modifiable copy of trip list
		this->depotList[depotID]->initAvailableTrips();
	}
}

void ProblemInstance::printInitialTripList() {
	cout << "\n---------- Initial Trip List ----------" << endl;
	for(Trip * current: this->initialTripList)
	{
		current->print();
		cout << "----" << endl;
	}

}

//Generate trip list from Assembly Points to Shelters
void ProblemInstance::generateTripList()
{
	int totalAP = this->assemblyPointList.size();
	int totalShelters = this->shelterList.size();

	/*create lexicographic ordering
	 *
	 * Trip  Assembly Point	 Shelter
	 * 	0	 	  0				0
	 * 	1	 	  0				1
	 * 	2		  0				2
	 * 	3		  1				0
	 * 	4		  1				1
	 * 	5		  1				2
	 * 	...		 ...			...
	 * */

	int totalTrips = totalShelters * totalAP;
	int tripID=0;
	for (int apID=0; apID<totalAP; apID++)
	{
		for (int shelterID=0; shelterID<totalShelters; shelterID++)
		{
			//send pointer to depot and assembly point to constructor
			Trip * newTrip = new Trip(tripID, this->assemblyPointList[apID],this->shelterList[shelterID],false);

			//add tour to initial tour list
			this->tripList.push_back(newTrip);

			this->assemblyPointList[apID]->addNeighbor(newTrip->getDistance(), newTrip->getEndPoint());
			this->assemblyPointList[apID]->addTrip(newTrip);

			this->shelterList[shelterID]->addNeighbor(newTrip->getDistance(), newTrip->getStartPoint());
			this->shelterList[shelterID]->addTrip(newTrip);
			tripID++;
		}

		//make modifiable copy of trip list for Assembly Point
		this->assemblyPointList[apID]->initAvailableTrips();
	}

	for (int shelterID=0; shelterID<totalShelters; shelterID++)
	{
		//make modifiable copy of trip list for Shelter
		this->shelterList[shelterID]->initAvailableTrips();
	}
}

void ProblemInstance::resetBuses() {
	for(Bus * current: this->busList)
		current->reset();
}

void ProblemInstance::resetShelters() {
	for(Shelter * current: this->shelterList)
		current->reset();
}

void ProblemInstance::resetAssemblyPoints() {
	for(AssemblyPoint * current: this->assemblyPointList)
		current->reset();
}

void ProblemInstance::resetDepots() {
	for(Depot * current: this->depotList)
		current->reset();
}

float ProblemInstance::calculateOF() {
	float maxValue = 0;

	//The bus that makes the longest evacuation (in terms of distance/cost), has the value of the OF
	for(Bus * current:this->busList)
	{
		float currentCost = current->getTotalRouteCost();
		if(currentCost > maxValue)
			maxValue = currentCost;
	}

	return maxValue;
}

void ProblemInstance::resetSolution() {
	this->resetBuses();
	this->resetDepots();
	this->resetAssemblyPoints();
	this->resetShelters();

	this->totalAvailableShelterCapacity = this->totalShelterCapacity;
	this->totalPendingDemand = this->totalDemand;
}


Trip* ProblemInstance::getTrip(LocationNode* startingNode,LocationNode* endingNode) {

	if(startingNode->getNodeType()=="depot" && endingNode->getNodeType()=="assemblyPoint")
	{
		for(int i=0;i<this->initialTripList.size();i++)
		{
			if(startingNode->getID()==initialTripList[i]->getStartPoint()->getID() && endingNode->getID()==initialTripList[i]->getEndPoint()->getID())
				return initialTripList[i];
		}
	}
	else if(startingNode->getNodeType()=="assemblyPoint" && endingNode->getNodeType()=="shelter")
	{
		for(int i=0;i<this->tripList.size();i++)
		{
			if(startingNode->getID()==tripList[i]->getStartPoint()->getID() && endingNode->getID()==tripList[i]->getEndPoint()->getID())
				return tripList[i];
		}
	}

	return NULL;
}
