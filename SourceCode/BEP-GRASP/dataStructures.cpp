#include "dataStructures.hpp"
#include <iostream>
#include <stdexcept>


/*---------- BUS CLASS ----------*/

void Bus::print(ostream& file)
{
	file 	<< "BusID: " << this->ID << std::endl
			<< "Capacity: " << this->capacity << std::endl
			<< "Depot: " << this->startingDepot->getID() << std::endl;
}

void Bus::printTripList()
{
	//vector<Trip*>
	for(int i=0; i<this->tripList.size(); i++)
	{
		cout << "(" << tripList.at(i)->getStartPoint()->getNodeTypeShort() << tripList.at(i)->getStartPoint()->getID()
			 << "," << tripList.at(i)->getEndPoint()->getNodeTypeShort() << tripList.at(i)->getEndPoint()->getID() << ")-["
			 <<tripList.at(i)->getDistance() << "] ";
	}
	cout << "Cost: " << this->getTotalRouteCost() << endl;
}

/* Add trip to bus trip list, and change current position of bus*/
void Bus::addTrip(Trip *trip_)
{
	float addedDistance=0; //cost of traveling from last shelter visited to the AP of the new trip to add

	if(this->tripList.size()>1)
	{
		Shelter * lastShelterVisited = (Shelter *)this->tripList.back()->getEndPoint();
		addedDistance = lastShelterVisited->getDistanceToAP(trip_->getStartPoint()->getID());
	}
	
	//if(this->ID==0) cout << "Adding trip " << trip_->getString() << " with added distance " << addedDistance << "+" << trip_->getDistance()<< endl;

	//add trip
	this->tripList.push_back(trip_);
	//add costs of making a new trip to total cost
	this->totalRouteCost+= addedDistance + trip_->getDistance();

	//set current position
	this->currentLocation=trip_->getEndPoint();
}



bool Bus::addTripInPosition(Trip* newTrip, int position) {

	//only allow introducing a trip from position=1 to position=tripList.size
	if(position<1 || position > tripList.size()) return false;

	//first get the position
	vector<Trip*>::iterator it=tripList.begin();
	advance(it,position);

	float addedDistance=0; //cost of traveling from last shelter visited to the AP of the new trip to add


	if(tripList.size()>1) //at least 1 initial trip
	{
		//cout << "Inserting trip..." << endl;

		if(!tripList[position-1]->checkIfInitialTrip()) //if previous trip is not first trip, add distance from previous shelter to inserted ap
		{
			addedDistance += tripList[position-1]->getDistanceToNextTrip(newTrip);
			//cout << "AÑADIENDO DISTANCIA ADICIONAL ANTES DEL TRIP: +" << tripList[position-1]->getDistanceToNextTrip(newTrip) << endl;

			//if new trip is inserted in between two existing trips, remove link between them
			if(position>1 && position < tripList.size())
			{
				//deleting distance between previously connected trips
				addedDistance -= tripList[position-1]->getDistanceToNextTrip(tripList[position]);
			}
		}
		if(position<tripList.size())//position is in between
		{
			addedDistance += newTrip->getDistanceToNextTrip(tripList[position]);

			//cout << "AÑADIENDO DISTANCIA ADICIONAL DESPUES DEL TRIP: +" << newTrip->getDistanceToNextTrip(tripList[position]) << endl;
		}
	}


	//add trip
	this->tripList.insert(it,newTrip);

	//add costs of making a new trip to total cost
	this->totalRouteCost+= addedDistance + newTrip->getDistance();

	if(position==1)	this->repairInitialTrip();

	return true;
}


void Bus::repairInitialTrip() {
	/*ASSUME WE HAVE SOMETHING LIKE (D0 AP0) (AP1 S0), AP0 and AP1 are inconsistent. We will fix it to (D0 AP1) (AP1 S0)*/
	/*NEW*/
	if(tripList.size()>=2 && (tripList[0]->getEndPoint()->getID() != tripList[1]->getStartPoint()->getID()))
	{
		LocationNode * startingDepot = tripList[0]->getStartPoint();
		LocationNode * newFirstAP = tripList[1]->getStartPoint();

		//find new initial trip
		Trip * newInitialTrip = startingDepot->getTripToLocation(newFirstAP);
		this->editTripInTime(newInitialTrip,0);
	}

	return;

	/*END NEW*/

}


void Bus::editTripInTime(Trip *trip_,int time)
{
	this->totalRouteCost-=this->tripList[time]->getDistance(); //remove cost associated with previous trip in t
	this->tripList.at(time) = trip_; //change trip in t with new trip trip_
	this->totalRouteCost+=this->tripList[time]->getDistance(); //add cost associated with new trip
}

bool Bus::removeTripInPosition(int position)
{
	//only allow introducing a trip from position=1 to position=tripList.size
	if(position<1 || position > this->tripList.size() || this->tripList.size()<2) return false;

	//first get the position
	vector<Trip*>::iterator it=tripList.begin();
	advance(it,position);

	float removeDistance=0; //cost of traveling from last shelter visited to the AP of the new trip to add


	if(position<this->tripList.size()-1)
	{
		//remove distance of the trip to be deleted, and next distance to return to an ap
		removeDistance-=this->tripList[position]->getDistance(); //remove cost associated to trip t
		removeDistance-=this->tripList[position]->getDistanceToNextTrip(this->tripList[position+1]);//remove cost to go from removed trip to next trip
		if(position>1)
		{
			removeDistance -= tripList[position-1]->getDistanceToNextTrip(tripList[position]); //add previous distance
			removeDistance += tripList[position-1]->getDistanceToNextTrip(tripList[position+1]);//add new connection
		}
	}
	else
	{

		//deleting last element
		removeDistance-=tripList[position]->getDistance(); //remove cost associated to trip t
		if(!tripList[position-1]->checkIfInitialTrip())
			removeDistance-=tripList[position-1]->getDistanceToNextTrip(tripList[position]); //add previous distance
	}

	this->totalRouteCost+=removeDistance;
	this->tripList.erase(this->tripList.begin()+position);//remove trip t

	if(position==1)	this->repairInitialTrip(); //repair initial trip if necessary

	return true;
}


/*-----------------*/

void LocationNode::removeAvailableTrip(Trip* trip) {
	/*find first element in the list, with the same distance as the element to remove*/
	multimap<float,Trip*>::iterator foundPos = this->availableTrips.find(trip->getDistance());

	/*search element to delete only among trips with the same distance*/
	for(multimap<float,Trip*>::iterator it = foundPos; it != availableTrips.end();++it)
	{
		if((*it).first != (*foundPos).first)
			break; //the trips doesn't have the same distance key
		else if((*it).second->getID() == trip->getID())
		{
			//the trips have the same distance key, and the same trip element, so it's the one we want to remove
			availableTrips.erase(it);
			break;
		}
	}
}

void LocationNode::printTrips(int endLimitPosition)
{
	if(!this->tripsByDistance.empty())
	{
		multimap<float,Trip*>::iterator endLimit=this->tripsByDistance.begin();
		if(endLimitPosition!=-1)
			advance(endLimit,endLimitPosition);
		else
			endLimit = this->tripsByDistance.end();

		cout << "---Feasible trips from this node" << endl;
		for(multimap<float,Trip*>::iterator it = this->tripsByDistance.begin(); it != endLimit;++it)
		{
			Trip * auxTrip = (*it).second;
			cout << "ID: " << auxTrip->getID()
				 << "\t- Start: " << auxTrip->getStartPoint()->nodeType << " " << auxTrip->getStartPoint()->getID()
				 << "\t- End: " << auxTrip->getEndPoint()->nodeType << " " << auxTrip->getEndPoint()->getID()
				 << " - Distance: " << auxTrip->getDistance() << endl;

			if(it == this->tripsByDistance.end())
				break;
		}
	}
	else
		cout << "---NO FEASIBLE TRIPS" << endl;
}

void LocationNode::printAvailableTrips(int endLimitPosition)
{
	if(!this->availableTrips.empty())
	{
		multimap<float,Trip*>::iterator endLimit=this->availableTrips.begin();
		if(endLimitPosition!=-1)
			advance(endLimit,endLimitPosition);
		else
			endLimit = this->availableTrips.end();

		cout << "---Feasible trips from this node" << endl;
		for(multimap<float,Trip*>::iterator it = this->availableTrips.begin(); it != endLimit;++it)
		{
			Trip * auxTrip = (*it).second;
			cout << "ID: " << auxTrip->getID()
				 << "\t- Start: " << auxTrip->getStartPoint()->nodeType << " " << auxTrip->getStartPoint()->getID()
				 << "\t- End: " << auxTrip->getEndPoint()->nodeType << " " << auxTrip->getEndPoint()->getID()
				 << " - Distance: " << auxTrip->getDistance() << std::endl;

			if(it==availableTrips.end())
				break;
		}
	}
	else
		cout << "---NO FEASIBLE TRIPS" << endl;
}

void LocationNode::addTrip(Trip* trip) {
	this->tripsByDistance.insert(std::pair<float,Trip*>(trip->getDistance(),trip));
}

void LocationNode::addAvilableTrip(Trip* trip)
{
	this->availableTrips.insert(std::pair<float,Trip*>(trip->getDistance(),trip));
}

void LocationNode::removeAvailabletripByStartPoint(LocationNode* start)
{
	if(!this->availableTrips.empty())
	{
		for(multimap<float,Trip*>::iterator it = this->availableTrips.begin(); it != this->availableTrips.end();++it)
		{
			Trip * auxTrip = (*it).second;
			if(auxTrip->getStartPoint()->getID() == start->getID())
			{
				//cout << "borrar\n";
				this->removeAvailableTrip(auxTrip);
			}
		}
	}
}

void LocationNode::removeAvailabletripByEndPoint(LocationNode* end)
{
	if(!this->availableTrips.empty())
	{
		for(multimap<float,Trip*>::iterator it = this->availableTrips.begin(); it != this->availableTrips.end();++it)
		{
			Trip * auxTrip = (*it).second;
			if(auxTrip->getEndPoint()->getID() == end->getID())
			{
				//cout << "borrar\n";
				this->removeAvailableTrip(auxTrip);
			}
		}
	}
}

Trip* LocationNode::getTripToLocation(LocationNode* endingNode)
{
	for(multimap<float,Trip*>::iterator it = this->tripsByDistance.begin(); it != this->tripsByDistance.end();++it)
	{
		Trip * currentTrip = (*it).second;
		if(endingNode->getID()==currentTrip->getEndPoint()->getID())
		{
			//cout << "Trip found!" << endl;
			return currentTrip;
		}
	}
	return NULL;
}

void LocationNode::printNeighbors(int endLimitPosition)
{
	map<float,LocationNode*>::iterator endLimit=this->neighborByDistance.begin();
	if(endLimitPosition!=-1)
		advance(endLimit,endLimitPosition);
	else
		endLimit = this->neighborByDistance.end();

	cout << "---Neighbors By Distance" << endl;
	for(map<float,LocationNode*>::iterator it = this->neighborByDistance.begin(); it != endLimit;++it)
	{
		cout <<"Neighbor ID: " << (*it).second->getID()
			 << "\t- Type: " <<  (*it).second->getNodeType()
			 << "\t- Distance: " << (*it).first << endl;
		if(it == this->neighborByDistance.end())
			break;
	}
}

/*---------- DEPOT CLASS ----------*/

void Depot::print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file)
{
	file 	<< "DepotID: " << ID << std::endl
			<< "Buses in Depot: " << this->busesInDepot << std::endl;

	if(showDistanceMatrix)
	{
		file << "--Distance Matrix" << endl;
		for(int i=0; i<this->distanceToAP.size();i++)
			file << "\tDistance to AP " << i << ": " << this->distanceToAP[i] << std::endl;
	}

	if(showNeighborMultimap)
	{
		file << "--Neighbors By Distance" << endl;
		for(map<float,LocationNode*>::iterator it = this->neighborByDistance.begin(); it != this->neighborByDistance.end();++it)
			   file << "\tNeighbor Assembly Point ID: " << (*it).second->getID()
			   	   	   << "\tDistance to neighbor: " << (*it).first << std::endl;
	}

}


/*---------- ASSEMBLY POINT CLASS ----------*/

void AssemblyPoint::print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file)
{
	file 	<< "AssemblyPointID: " << this->ID << std::endl
			<< "Demand: " << this->totalDemand << std::endl;

	if(showDistanceMatrix)
	{
		file << "--Distance Matrix" << endl;
		for(int i=0; i<this->distanceToShelter.size() ;i++)
			file << "\tDistance to Shelter " << i << ": " << this->distanceToShelter[i] << std::endl;
	}

	if(showNeighborMultimap)
	{
		file << "--Neighbors By Distance" << endl;
		for(map<float,LocationNode*>::iterator it = this->neighborByDistance.begin(); it != this->neighborByDistance.end();++it)
			   file << "\tNeighbor Shelter ID: " << (*it).second->getID()
					   << "\tDistance to neighbor: " << (*it).first << std::endl;
	}
}




/*---------- SHELTER CLASS ----------*/

void Shelter::print(bool showDistanceMatrix, bool showNeighborMultimap,ostream& file)
{
	file 	<< "ShelterID: " << this->ID << std::endl
			<< "Capacity: " << this->totalCapacity << std::endl;

	if(showDistanceMatrix)
	{
		file << "--Distance Matrix" << endl;
		for(int i=0; i<this->distanceToAP.size();i++)
			file << "\tDistance to AP " << i << ": " << this->distanceToAP[i] << std::endl;
	}

	if(showNeighborMultimap)
	{
		file << "--Neighbors By Distance" << endl;
		for(map<float,LocationNode*>::iterator it = this->neighborByDistance.begin(); it != this->neighborByDistance.end();++it)
			   file << "\tNeighbor Assembly Point ID: " << (*it).second->getID()
					   << "\tDistance to neighbor: " << (*it).first << std::endl;
	}
}


void Depot::reset() {
	this->initAvailableTrips();
}

void AssemblyPoint::reset() {
	this->initAvailableTrips();
	this->pendingDemand = this->totalDemand;
}

void Shelter::reset() {
	this->initAvailableTrips();
	this->availableCapacity=this->totalCapacity;
}

void Bus::reset() {
	this->currentLocation=this->startingDepot;
	this->tripList.clear();
	totalRouteCost = 0;
}
