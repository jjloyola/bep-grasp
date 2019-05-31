#include "dataStructures.hpp"
#include "solution.hpp"
#include <algorithm>    // std::sort
#include <string>
#include <math.h>  //fabs

float Solution::calculateOF() {

	float maxValue = 0;

	//The bus that makes the longest evacuation (in terms of distance/cost), has the value of the OF
	for(int i=0;i<this->busList.size();i++)
	{
		Bus current = this->busList[i];
		float currentCost = current.getTotalRouteCost();
		if(currentCost > maxValue)
		{
			maxValue = currentCost;
			this->largestRouteBus = &(this->busList[i]);
		}
	}

	this->objFuncValue = maxValue;

	return maxValue;

}

//------------------------------------------------------------------------------------------------------------------------------------

bool bus1GreaterThanBus2ByRoute(Bus bus1, Bus bus2)
{
	return bus1.getTotalRouteCost() > bus2.getTotalRouteCost();
}

bool bus1LesserThanBus2ByID(Bus bus1, Bus bus2)
{
	return bus1.getID() < bus2.getID();
}

//------------------------------------------------------------------------------------------------------------------------------------

float Solution::calculateOFSorting() {

	std::sort(busList.begin(),busList.end(),bus1GreaterThanBus2ByRoute);

	return busList.at(0).getTotalRouteCost();
}

//------------------------------------------------------------------------------------------------------------------------------------

void Solution::sortBusesByID() {
	std::sort(busList.begin(),busList.end(),bus1LesserThanBus2ByID);
}

//------------------------------------------------------------------------------------------------------------------------------------

void Solution::print()
{
	cout << "------SOLUTION------" << endl;
	for (int i=0; i<this->busList.size(); i++)
	{
		cout << "Bus " << busList.at(i).getID() << ": ";
		busList.at(i).printTripList();
	}
}


//------------------------------------------------------------------------------------------------------------------------------------


bool Solution::checkFeasibility(ProblemInstance * problemInstance,bool printSuccess, bool printFailure) {


	//ASSUMING THAT EVERY TRIP MOVES BUS_CAPACITY PEOPLE FROM AP TO SHELTER

	vector<int> apList,shelterList; //each element contains the number of times visited for a bus

	for(int i=0; i < problemInstance->getAssemblyPointList().size();i++)
		apList.push_back(0);

	for(int i=0; i< problemInstance->getShelterList().size();i++)
		shelterList.push_back(0);


	for(int busID=0;busID < this->busList.size(); busID++) //for each bus
	{

		Bus * currentBus = &this->busList.at(busID);
		float routeCost = 0;
		string routeCostsString="";

		//*check triplist contains at least 1 trips (meaning that it participates on the evacuation and ends on a shelter)
		if(currentBus->getTripList().size()>=1)
		{
			//cout << "checking bus with trip list: "; currentBus->printTripList();
			for(int tripPosition=0;tripPosition < currentBus->getTripList().size(); tripPosition++) //*for each trip made by the busID
			{
				Trip * trip = currentBus->getTrip(tripPosition);

				routeCostsString += std::to_string(trip->getDistance()) + "+";
				routeCost += trip->getDistance();

				if(tripPosition < currentBus->getTripList().size()-1)
				{
					//add the inferred distance between trips
					routeCostsString += std::to_string(trip->getDistanceToNextTrip(currentBus->getTrip(tripPosition+1))) + "+";
					routeCost += trip->getDistanceToNextTrip(currentBus->getTrip(tripPosition+1));
				}

				if(tripPosition!=0)
				{
					apList[trip->getStartPoint()->getID()] += currentBus->getCapacity(); //*check amount of people picked from AP
					shelterList[trip->getEndPoint()->getID()] += currentBus->getCapacity(); //*amount of people left on Shelter
				}
			}
			routeCostsString.pop_back();

			if(fabs(routeCost-currentBus->getTotalRouteCost()) > 0.005f)
			{
				if(printFailure) cout << "El costo de ruta del bus " << currentBus->getID() << " no coincide con el costo calculado --> " << routeCostsString << "=" << routeCost <<
						"(currentBus->getTotalRouteCost()=" << currentBus->getTotalRouteCost() << ")" << endl;
				return false;
			}
			else
				if(printSuccess) cout << "El costo de ruta del bus " << currentBus->getID() << " ES CORRECTO!!! --> " << routeCostsString << "=" << routeCost << endl;
		}
	}

	for(int i=0; i < problemInstance->getAssemblyPointList().size();i++)
		if(apList[i] != problemInstance->getAssemblyPoint(i)->getTotalDemand()) //total demand was not satisfied
		{
			if(printFailure) cout << "Total demand in AP "<<i<<" was not satisfied" << endl;
			return false;
		}

	for(int i=0; i< problemInstance->getShelterList().size();i++)
		if(shelterList[i] > problemInstance->getShelter(i)->getTotalCapacity()) //shelter capacity was exceeded
		{
			if(printFailure) cout << "Shelter " << i <<" capacity was exceeded" << endl;
			return false;
		}



	if(printSuccess) cout << "Feasible Solution!"<< endl;
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------

float Solution::getLargestRouteCost()
{
	float largest = this->busList[0].getTotalRouteCost();
	for(int i=1; i<this->busList.size(); i++)
	{
		if(this->busList[i].getTotalRouteCost() > largest)
			largest = this->busList[i].getTotalRouteCost();
	}

	return largest;
}

//------------------------------------------------------------------------------------------------------------------------------------

float Solution::getShortestRouteCost()
{
	float shortest = this->busList[0].getTotalRouteCost();
	for(int i=1; i<this->busList.size(); i++)
	{
		if(this->busList[i].getTotalRouteCost() < shortest)
			shortest = this->busList[i].getTotalRouteCost();
	}

	return shortest;
}

//------------------------------------------------------------------------------------------------------------------------------------

void Solution::calculateAvgTimes(float speedInKmMin, int instanceTotalDemand)
{
	this->avgTimeToGetOnABus = 0;
	this->avgTimeToGetOnAShelter = 0;

	for(int busID=0; busID<this->busList.size(); ++busID)
	{

		Bus aux(busID, this->busList[busID].getCapacity(), this->busList[busID].getDepot());

		for(int tr=0; tr<this->busList[busID].getTripList().size(); ++tr)
		{
			aux.addTrip(this->busList[busID].getTrip(tr));
			//*time waited for each person (distance/speed) per amount of people loaded to the bus

			this->avgTimeToGetOnABus +=
					((aux.getTotalRouteCost()-aux.getTrip(tr)->getDistance())/speedInKmMin) * aux.getCapacity();

			this->avgTimeToGetOnAShelter +=
					(aux.getTotalRouteCost()/speedInKmMin) * aux.getCapacity();

		}
	}

	this->avgTimeToGetOnABus = this->avgTimeToGetOnABus/instanceTotalDemand;
	this->avgTimeToGetOnAShelter = this->avgTimeToGetOnAShelter/instanceTotalDemand;

}



