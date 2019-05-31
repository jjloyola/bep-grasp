/********************************
 * dataStructures.hpp
 *
 * Contains the definitions of the basic elements of the problem (Bus, Depot, Assembly Point and Shelter classes)
 *
 * */

#ifndef DATASTRUCTURES_HPP_
#define DATASTRUCTURES_HPP_
#include <vector>
#include <ostream>
#include <iostream>
#include <map>
#include <string>
#include "tripMapping.hpp"

using namespace std;

class Bus;
class Depot;
class AssemblyPoint;
class Shelter;
class InitialTrip;
class Trip;


class LocationNode
{
protected:
	int ID;
	string nodeType;
	string nodeTypeShort;
	multimap<float,LocationNode*> neighborByDistance;

	//list of trips that can be made from current location node to a neighbor, ordered by distance
	multimap<float,Trip*> tripsByDistance;
	multimap<float,Trip*> availableTrips;//a copy of the trip list, that can be modified

public:
	LocationNode(int ID_,string nodeType_,string nodeTypeShort_):ID(ID_),nodeType(nodeType_),nodeTypeShort(nodeTypeShort_){}
	virtual ~LocationNode(){}

	//GETTERS
	int & getID(){return this->ID;}
	string getNodeType(){return this->nodeType;}
	string getNodeTypeShort(){return this->nodeTypeShort;}
	multimap<float,Trip*> &getTrips(){return this->tripsByDistance;}
	multimap<float,Trip*> getTripsCopy(){return this->tripsByDistance;}
	multimap<float,Trip*> &getAvailableTrips(){return this->availableTrips;}
	Trip * getTripToLocation(LocationNode * endingNode);

	//SETTERS
	void addTrip(Trip * trip);

	void initAvailableTrips(){availableTrips=this->getTripsCopy();}
	void addAvilableTrip(Trip* trip);
	void removeAvailableTrip(multimap<float,Trip*>::iterator it){availableTrips.erase(it);}
	void removeAvailableTrip(Trip* trip);
	void removeAvailabletripByStartPoint(LocationNode * start);
	void removeAvailabletripByEndPoint(LocationNode * end);
	//void removeAvailableNeighbor(LocationNode )
	//void initAvailableNeighbors(){}

	//PRINT
	void printTrips(int endLimitPosition=-1);
	void printAvailableTrips(int endLimitPosition=-1);
	void printNeighbors(int endLimitPosition=-1);

	virtual void addNeighbor(float distance, LocationNode * neighbor)=0;
	//virtual void addTrip(float distance, Trip * trip)=0;

};


class Depot : public LocationNode
{
	int busesInDepot;
	vector<Bus*> busList;
	vector<float> distanceToAP;//distances from current Depot to each Assembly Point
public:
	//constructor & destructor
	Depot(int ID_,int busesInDepot_):LocationNode(ID_,"depot","D"),busesInDepot(busesInDepot_){}
	~Depot(){}

	//getters
	//int & getID(){return this->ID;}
	vector<Bus*> & getBusList(){return this->busList;}
	vector<float> & getDistanceToAPList(){return this->distanceToAP;}
	float & getDistanceToAP(int assemblyPointID){return this->distanceToAP[assemblyPointID];}

	//setters
	void addBus(Bus *bus_){this->busList.push_back(bus_);}
	void addDistanceToAP(float & distance_){this->distanceToAP.push_back(distance_);}
	void addNeighbor(float distance, LocationNode * neighbor)
			{this->neighborByDistance.insert(std::pair<float,LocationNode*>(distance,neighbor));}
	//void addTrip(float distance, Trip * trip) {this->tripsByDistance.insert(std::pair<float,Trip*>(distance,trip));}

	//other methods
	void print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file = std::cout);
	void reset();
};


class AssemblyPoint : public LocationNode
{
	int totalDemand;
	int pendingDemand;
	vector<float> distanceToShelter; //distances from current Assembly Point to each Shelter
	//multimap<float,Shelter*> neighborByDistance;
public:
	//constructor & destructor
	AssemblyPoint(int ID_, int demand_):LocationNode(ID_,"assemblyPoint","AP"),totalDemand(demand_),pendingDemand(demand_){}
	~AssemblyPoint(){}

	//getters
	//int & getID(){return this->ID;}
	int & getTotalDemand(){return this->totalDemand;}
	int & getCurrentDemand(){return this->pendingDemand;}
	vector<float> & getDistanceToShelterList(){return this->distanceToShelter;}
	float & getDistanceToShelter(int shelterID){return this->distanceToShelter[shelterID];}

	//setters
	void addDistanceToShelter(float & distance_){this->distanceToShelter.push_back(distance_);}
	void addNeighbor(float distance, LocationNode * neighbor)
				{this->neighborByDistance.insert(std::pair<float,LocationNode*>(distance,neighbor));}
	//void addTrip(float distance, Trip * trip) {this->tripsByDistance.insert(std::pair<float,Trip*>(distance,trip));}
	int reducePendingDemand(int variation)
	{
		this->pendingDemand-=variation;
		if(pendingDemand<0)pendingDemand=0;
		return this->pendingDemand;}
	int addPendingDemand(int variation){this->pendingDemand+=variation;return this->pendingDemand;}

	//other methods
	void print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file = std::cout);
	void reset();
};


class Shelter : public LocationNode
{
	int totalCapacity;
	int availableCapacity;
	vector<float> distanceToAP; //distances from current Shelter to each Assembly Point
	//multimap<float,AssemblyPoint*> neighborByDistance;
public:
	//constructor & destructor
	Shelter(int ID_, int capacity_):LocationNode(ID_,"shelter","S"),totalCapacity(capacity_),availableCapacity(capacity_){}
	~Shelter(){}

	//getters
	//int & getID(){return this->ID;}
	int & getTotalCapacity(){return this->totalCapacity;}
	int & getCurrentCapacity(){return this->availableCapacity;}
	vector<float> & getDistanceToAPList(){return this->distanceToAP;}
	float & getDistanceToAP(int assemblyPointID){return this->distanceToAP[assemblyPointID];}
	float & getDistanceToAP(AssemblyPoint * assemblyPoint){return this->distanceToAP[assemblyPoint->getID()];}
	multimap<float,LocationNode*> & getNeighbors(){return this->neighborByDistance;}

	//setters
	void addDistanceToAP(float & distance_){this->distanceToAP.push_back(distance_);}
	void addNeighbor(float distance, LocationNode * neighbor)
				{this->neighborByDistance.insert(std::pair<float,LocationNode*>(distance,neighbor));}
	//void addTrip(float distance,Trip * trip){}
	int reduceAvailableCapacity(int variation){this->availableCapacity-=variation;if(availableCapacity<0)availableCapacity=0;return this->availableCapacity;}
	int addAvailableCapacity(int variation){this->availableCapacity+=variation;return this->availableCapacity;}
	void removeNeighbor(LocationNode * neighbor);

	//other methods
	void print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file = std::cout);
	void reset();
};



class Bus
{
	int ID;
	int capacity;
	//LocationNode current; //at the beginning will be a depot
	Depot * startingDepot; //starting point
	LocationNode * currentLocation;
	//used to track the location, first element can be "depot","ap" or "shelter", second element is the id of the node where the bus is.
	//std::pair <string,int> currentNodeID;

	vector<Trip*> tripList;
	float totalRouteCost; //cost of the full route asigned (all trips)

public:
	//getters
	int & getID(){return this->ID;}
	int & getCapacity(){return this->capacity;}
	Depot * getDepot(){return this->startingDepot;}
	LocationNode * getCurrentLocation(){return this->currentLocation;}
	float getTotalRouteCost(){return this->totalRouteCost;}
	vector<Trip*> & getTripList(){return this->tripList;}
	Trip* & getTrip(int position){return this->tripList.at(position);}

	//setters
	//void setInitialTrip(InitialTrip * trip_){this->initialTrip=trip_;}
	void setCurrentLocation(LocationNode * location){this->currentLocation=location;}
	//edit trip list
	void addTrip(Trip *trip_);
	bool addTripInPosition(Trip *trip_,int position);
	void changeInitialTrip(Trip *newInitialTrip);
	void editTripInTime(Trip *trip_,int time);
	bool removeTripInPosition(int position);
	void resetTripList(){this->tripList.clear();}
	void reset();
	void repairInitialTrip();

	//constructor
	Bus(int ID_, int capacity_, LocationNode * depot_):ID(ID_),capacity(capacity_),startingDepot((Depot *) depot_),currentLocation(depot_),totalRouteCost(0){}

	//PRINT
	void print(ostream& file = std::cout);
	void printTripList();
};




#endif /* DATASTRUCTURES_HPP_ */
