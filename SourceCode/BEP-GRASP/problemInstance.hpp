/********************************
 * problemInstance.hpp
 *
 * Contains all the necessary objects to have an instance of the problem.
 * Connects all the basic objects to create a scenario.
 *
 * */

#ifndef PROBLEMINSTANCE_HPP_
#define PROBLEMINSTANCE_HPP_
#include "dataStructures.hpp"
#include "tripMapping.hpp"
#include <map>

class ProblemInstance
{
	int totalDemand;
	int totalShelterCapacity;
	int totalPendingDemand; //demand left to serve
	int totalAvailableShelterCapacity; //available capacity
	int busCapacity;

	vector<Bus*> busList;
	vector<Depot*> depotList;
	vector<AssemblyPoint*> assemblyPointList;
	vector<Shelter*> shelterList;
	vector<Trip*> initialTripList;
	vector<Trip*> tripList;

	multimap<int,AssemblyPoint*> assemblyPointsByDemand;
	multimap<int,Shelter*> sheltersByCapacity;

	void resetBuses();
	void resetShelters();
	void resetAssemblyPoints();
	void resetDepots();

public:
	//getters
	vector<Bus*> & getBusList(){return this->busList;}
	vector<Bus*> getBusListCopy(){return this->busList;}
	vector<Depot*> & getDepotList(){return this->depotList;}
	vector<AssemblyPoint*> & getAssemblyPointList(){return this->assemblyPointList;}
	vector<Shelter*> & getShelterList(){return this->shelterList;}
	vector<Trip*> & getTripList(){return this->tripList;}
	Bus * getBus(int busID){return this->busList[busID];}
	Depot * getDepot(int depotID){return this->depotList[depotID];}
	AssemblyPoint * getAssemblyPoint(int apID){return this->assemblyPointList[apID];}
	Shelter * getShelter(int shelterID){return this->shelterList[shelterID];}
	Trip * getTrip(int tripID){return this->tripList[tripID];}
	Trip * getTrip(LocationNode * startingNode,LocationNode * endingNode);
	int getTotalDemand(){return this->totalDemand;}
	int getTotalShelterCapacity(){return this->totalShelterCapacity;}
	int getTotalPendingDemand(){return this->totalPendingDemand;}
	int getTotalAvailableShelterCapacity(){return this->totalAvailableShelterCapacity;}
	int getBusCapacity(){return this->busCapacity;}

	float calculateOF();

	//setters
	void initTotalDemand(int demand_){this->totalDemand = demand_; this->totalPendingDemand = demand_;}
	void initTotalShelterCapacity(int capacity_){this->totalShelterCapacity = capacity_; this->totalAvailableShelterCapacity = capacity_;}
	void reducePendingDemand(int value){this->totalPendingDemand-=value;}
	void reduceAvailableShelterCapacity(int value){this->totalAvailableShelterCapacity -= value;}
	void addBus(Bus *bus_){this->busList.push_back(bus_);}
	void addBus(int &busID, int &capacity, Depot * depot){this->busList.push_back(new Bus(busID,capacity,depot));}
	void addDepot(Depot *depot_){this->depotList.push_back(depot_);}
	void addDepot(int &depotID, int &busesInDepot){this->depotList.push_back(new Depot(depotID,busesInDepot));}
	void addAssemblyPoint(AssemblyPoint *ap_){this->assemblyPointList.push_back(ap_);}
	void addAssemblyPoint(int &assemblyPointID, int &demand){this->assemblyPointList.push_back(new AssemblyPoint(assemblyPointID,demand));}
	void addShelter(Shelter *shelter_){this->shelterList.push_back(shelter_);}
	void addShelter(int &shelterID, int &capacity){this->shelterList.push_back(new Shelter(shelterID,capacity));}
	void setBusCapacity(int cap){this->busCapacity=cap;}


	//multimaps control
	void addAssemblyPointByDemand(int demand_, AssemblyPoint* ap_){this->assemblyPointsByDemand.insert(std::pair<int,AssemblyPoint*>(demand_,ap_));}
	void addShelterByCapacity(int capacity_, Shelter* shelter_);

	//PRINT
	void printBusList();
	void printDepotList(bool showDistanceMatrix,bool showNeighborMultimap);
	void printAssemblyPointList(bool showDistanceMatrix,bool showNeighborMultimap);
	void printShelterList(bool showDistanceMatrix,bool showNeighborMultimap);
	void printInitialTripList();
	void printSolution();

	void printBusPositions();

	void generateInitialTripList();
	void generateTripList();

	void resetSolution();

};
#endif /* PROBLEMINSTANCE_HPP_ */
