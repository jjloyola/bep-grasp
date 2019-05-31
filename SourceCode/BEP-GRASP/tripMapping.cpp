#include "tripMapping.hpp"

#include "problemInstance.hpp"


Trip::Trip(int ID_, LocationNode * start_, LocationNode * end_, bool firstTrip)
{
	this->ID = ID_;
	this->startPoint = start_; //starting node (an assembly point)
	this->endPoint = end_; //ending node (a shelter)
	this->isInitialTrip = firstTrip;


	if(firstTrip)
	{
		//get distance from Depot to AssemblyPoint
		Depot * start = (Depot *)startPoint;
		AssemblyPoint * end = (AssemblyPoint*)endPoint;

		this->distance = start->getDistanceToAP(end->getID()); //distance from depot to assembly point
	}
	else
	{
		//get distance from AssemblyPoint to Shelter
		AssemblyPoint * start = (AssemblyPoint*)startPoint;
		Shelter * end = (Shelter*)endPoint;
		this->distance = start->getDistanceToShelter(end->getID()); //distance from assembly point to shelter
	}
}

void Trip::print(ostream& file)
{
	file 	<< "TripID: " << this->ID << endl
			<< "Starting Point ID: " << this->startPoint->getID() << endl
			<< "Ending Point ID: " << this->endPoint->getID() << endl
			<< "Distance: " << this->distance << endl;
}

float Trip::getDistanceToNextTrip(Trip* nextTrip) {
	if(this->endPoint->getNodeTypeShort() == "S")
		return ((Shelter *)this->endPoint)->getDistanceToAP(nextTrip->getStartPoint()->getID());
	else
		return 0;

}


string Trip::getString()
{
	string str = "("+ this->startPoint->getNodeTypeShort() + std::to_string(this->startPoint->getID()) + "," +
			this->endPoint->getNodeTypeShort() + std::to_string(this->endPoint->getID()) + ")";
	return str;
}
