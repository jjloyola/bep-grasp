#ifndef TRIPMAPPING_HPP_
#define TRIPMAPPING_HPP_
#include "dataStructures.hpp"
#include <string>

using namespace std;

class LocationNode;
class Bus;
class Depot;
class AssemblyPoint;
class Shelter;


class Trip
{
	int ID;
	LocationNode * startPoint;
	LocationNode * endPoint;
	float distance;
	bool isInitialTrip;

public:
	Trip(int ID_, LocationNode * start_, LocationNode * end_, bool isInitialTrip_);
	int & getID(){return this->ID;}
	LocationNode * getStartPoint(){return this->startPoint;}
	LocationNode * getEndPoint(){return this->endPoint;}
	float & getDistance(){return this->distance;}
	float getDistanceToNextTrip(Trip * nextTrip);
	bool & checkIfInitialTrip(){return this->isInitialTrip;}
	void print(ostream& file = std::cout);
	string getString();
};

#endif /* TRIPMAPPING_HPP_ */
