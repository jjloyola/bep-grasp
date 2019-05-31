#include "solver.hpp"
#include "problemInstance.hpp"
#include "dataStructures.hpp"
#include "tripMapping.hpp"
#include "solution.hpp"
#include <math.h>
#include <algorithm>
#include "mainHeader.hpp"
#include "utils.hpp"



#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)


Solution Solver::grasp(vector<string> * qualityEvolution)
{

	int currentIteration = 0;
	bool flag1 = false; //to save only one bestFO per second when saving evolution of quality

	while( ((clock() - this->startTime)<timeLimit) )/*&& (currentIteration < this->iterLimit) )*/
	{
		//cout << "\n************** GRASP ITERATION: "<<currentIteration <<"*********************" << endl;

		this->greedyRandomizedConstructor();
		currentSolution = this->getSolutionFromProblemInstance(); //create new object with new

		//if(!currentSolution->checkFeasibility(problemInstance,false,false))
		//	cout << "Solucion inicial infactible!!!!!!!!!!!!!!!!!\n" << endl;

		if(problemInstance->calculateOF() < bestSolution.getObjFuncValue())
		{
			//cout << "\nSAVING AS BEST SOLUTION....." << endl;
			this->setCurrentBestSolution();
			this->bestSolution.timeFound = (float)(clock() - this->startTime)/CLOCKS_PER_SEC;
			//cout << "*******************\nBest solution found at: " << this->bestSolution.timeFound << " seconds - ";
			//cout << "Value: " << this->bestSolution.getObjFuncValue() << endl;
		}

		//if(!currentSolution->checkFeasibility(problemInstance,false)) \
			currentSolution->print();


		this->localSearch();

		//cout << "*--------Objective function value: " << problemInstance->calculateOF() << "--------*" << endl;
		//problemInstance->printSolution();

		this->problemInstance->resetSolution();

		//------TO SAVE THE EVOLUTION OF THE QUALITY VS TIME, WITH INPUT PARAMETER -saveQualityEvol---
		/*
		if(  (qualityEvolution->size() > 0) && ( ((clock() - startTime) / (int)CLOCKS_PER_SEC) % 5 == 0)   )
		{
			if(!flag1)
			{
				qualityEvolution->push_back(  to_string((clock() - startTime)/(int)CLOCKS_PER_SEC) + ";" + to_string(this->bestSolution.getObjFuncValue())  );
				flag1 = true;
			}
		}
		else flag1 = false;
		*/
		//---------------------------------------------------------------------------------------------

		delete currentSolution;
		++currentIteration;

	}

	/*
	cout << "\n************************************************************************************" << endl;
	cout << "*                                  BEST SOLUTION                                   *" << endl;
	cout << "*                                                                                  *" << endl;
	cout << "*--------Objective function value: " << bestSolution.getObjFuncValue() << "--------*" << endl;
		bestSolution.print();
		cout << endl;
	 */


	return this->bestSolution;
}



Solution Solver::graspCplexComp(float cplexResult)
{

	int currentIteration = 0;
#if CPLEX_COMP==1
	bool cplexReached = false;
#endif

	while( ((clock() - this->startTime)<timeLimit) )/*&& (currentIteration < this->iterLimit) )*/
	{
		//cout << "\n************** GRASP ITERATION: "<<currentIteration <<"*********************" << endl;

		this->greedyRandomizedConstructor();
		currentSolution = this->getSolutionFromProblemInstance(); //create new object with new

		if(problemInstance->calculateOF() < bestSolution.getObjFuncValue())
		{
			//cout << "\nSAVING AS BEST SOLUTION....." << endl;
			this->setCurrentBestSolution();
			this->bestSolution.timeFound = (double)(clock() - this->startTime)/CLOCKS_PER_MS;
			this->timeToReachCplex = this->bestSolution.timeFound;
			//cout << "Best solution found at: " << this->bestSolution.timeFound << " seconds - ";
			//cout << "Value: " << this->bestSolution.getObjFuncValue() << endl;
		}


		this->localSearch();



#if CPLEX_COMP==1

		//------------FOR CPLEX COMPARISON---------------------------------------
		if(!cplexReached && utils::double_leq(this->bestSolution.getObjFuncValue()/cplexResult,(double)1.000,TOLERANCE))
		{
			this->timeToReachCplex = (double)(clock() - this->startTime)/CLOCKS_PER_MS;
			cplexReached = true;
			#if STOP_WHEN_REACHED==1
				break;
			#endif
		}

		//-----------------------------------------------------------------------
#endif

		//cout << "*--------Objective function value: " << problemInstance->calculateOF() << "--------*" << endl;
		//problemInstance->printSolution();

		this->problemInstance->resetSolution();


		delete currentSolution;
		++currentIteration;

	}

	/*
	cout << "\n************************************************************************************" << endl;
	cout << "*                                  BEST SOLUTION                                   *" << endl;
	cout << "*                                                                                  *" << endl;
	cout << "*--------Objective function value: " << bestSolution.getObjFuncValue() << "--------*" << endl;
		bestSolution.print();
		cout << endl;
	 */

	return this->bestSolution;
}




/*If 0<'size'<1, its taken as a percentage of the original trip list size*/
void Solver::greedyRandomizedConstructor() {

	bool firstTrip=true;
	bool evacuationCompleted=false;
	int refListSize=1; //at least 1

	assignDepoApTrip(); /*Make trip from Depot to AP, for all buses*/
	vector<Bus*> busList = problemInstance->getBusListCopy();

	while(problemInstance->getTotalPendingDemand() != 0)//(!evacuationCompleted)
	{
		std::random_shuffle(busList.begin(),busList.end());

		for(Bus * currentBus:busList)
		{

			//cout << "**Bus " << currentBus->getID() << "**" << endl;

			/*see where is the bus (LocationNode)*/
			LocationNode * currentNode = currentBus->getCurrentLocation();
			//cout << "---Current Location: " << currentNode->getNodeType() << " " << currentNode->getID() << endl;

			/*get the list with possible trip from that location node*/
			multimap<float,Trip*> tripList = currentNode->getAvailableTrips();
			//currentNode->printAvailableTrips();

			if(tripList.empty())
			{
				//cout << "EVACUATION COMPLETED" << endl;
			}
			else {
				/*refListSize has the proportion of the trip list considered for the reference list*/
				int refListSize = ceil(tripList.size()*refListProportion);

				/*Generate number between 0 and refListSize-1 (to select a trip from the reference list)*/
				int steps = rand() % refListSize;

				std::multimap<float,Trip*>::iterator it=tripList.begin();
				advance(it,steps);
				Trip * selectedTrip = (*it).second;

				//cout << "---Selected trip ID:" << selectedTrip->getID() << endl;
				AssemblyPoint * ap = (AssemblyPoint*)selectedTrip->getStartPoint();
				Shelter * shelter = (Shelter*)selectedTrip->getEndPoint();

				if(currentNode->getNodeType()!="shelter")
				{
					/*ASSEMBLY POINT*/

					//cout << "---Available capacity on Shelter "<< shelter->getID() <<": " << shelter->getCurrentCapacity() << endl;

					/*add trip to bus*/
					currentBus->addTrip(selectedTrip);
					//cout << "Adding trip (AP" << selectedTrip->getStartPoint()->getID()
					//				<< ",S" << selectedTrip->getEndPoint()->getID()<< ") to bus..." << endl;


					//move people from AssemblyPoint to Shelter, and check
					//If Shelter in this trip doesnt have more capacity, remove the trips that connect any AP to this shelter
					if(shelter->reduceAvailableCapacity(currentBus->getCapacity()) == 0)
					{
						//remove trips to shelter from all Assembly Point
						for(int i=0;i<problemInstance->getAssemblyPointList().size(); i++)
						{
							//remove all trips with endPoint = current shelter
							problemInstance->getAssemblyPoint(i)->removeAvailabletripByEndPoint(shelter);
						}
					}

					//cout << "---Updated Available capacity on Shelter "<< shelter->getID() <<": " << shelter->getCurrentCapacity() << endl;

					//Once the people are on the shelter, we can reduce the Total Pending Demand
					problemInstance->reducePendingDemand(currentBus->getCapacity());
					problemInstance->reduceAvailableShelterCapacity(currentBus->getCapacity());
				}
				else
				{
					/*SHELTER*/

					//cout << "---Demand on AP " << ap->getID() << ": " << ap->getCurrentDemand() << endl;


					/*set current location to selected neighbor, the distance will be added on the next iteration
					 * when adding the next trip*/
					currentBus->setCurrentLocation(ap);
					//cout << "Updating current location to " << ap->getNodeType() << " "
					//	 << ap->getID() << "..." << endl;

					/*get people on the bus (reduce pending demand on assembly point) and check if there are more people
					 * left to evacuate, for the next buses and iterations*/
					if(ap->reducePendingDemand(currentBus->getCapacity()) == 0)
					{
						//There are no more people on the Assembly Point, remove the trips that connect this AP with
						//any Shelter
						for(int i=0;i<problemInstance->getShelterList().size(); i++)
						{
							//remove all trips with startPoint = current AP
							problemInstance->getShelter(i)->removeAvailabletripByStartPoint(ap);
						}
					}

					//cout << "---Updated Demand on AP " << ap->getID() << ": " << ap->getCurrentDemand() << endl;
				}
			}

			//cout << "--------------------\n" << endl;
		}

		//cout << "---> Total Demand: " << problemInstance->getTotalDemand()
		//						<< " - Pending Demand: " << problemInstance->getTotalPendingDemand() << endl;
		//cout << "---> Total Shelter Capacity: " << problemInstance->getTotalShelterCapacity()
		//						<< " - Available Shelter Demand: " << problemInstance->getTotalAvailableShelterCapacity() << endl;

		//this->problemInstance->printBusPositions();
		//cout << "-------------------------------------------------------------------------\n\n";
	}

	return ;
}



void Solver::assignDepoApTrip()
{
	/*Only used to add trip from Depots to Assembly Point*/
	int refListSize=1; //at least 1

	for(Bus * currentBus:problemInstance->getBusList())
	{

		//cout << "**Bus " << currentBus->getID() << "**" << endl;

		/*see where is the bus (LocationNode)*/
		LocationNode * currentNode = currentBus->getCurrentLocation();
		//cout << "* Current Location: " << currentNode->getNodeType() << " " << currentNode->getID() << endl;

		/*get the list with possible trip from that location node*/
		multimap<float,Trip*> tripList = currentNode->getAvailableTrips();
		//currentNode->printAvailableTrips();

		/*refListSize has the proportion of the trip list considered for the reference list*/
		int refListSize = ceil(tripList.size()*refListProportion);

		//cout << "* Ref List Size: " << refListSize << endl;
		//currentNode->printAvailableTrips(refListSize);//print until position refListSize

		/*Generate number between 0 and refListSize-1 (to select a trip from the reference list)*/
		int steps = rand() % refListSize;
		//cout << "* Selected trip position: " << steps << endl;

		std::multimap<float,Trip*>::iterator it=tripList.begin();
		advance(it,steps);
		Trip * selectedTrip = (*it).second;

		//cout << "* Selected trip ID:" << selectedTrip->getID() << endl;
		Depot * depot = (Depot*)selectedTrip->getStartPoint();
		AssemblyPoint * ap = (AssemblyPoint*)selectedTrip->getEndPoint();

		//cout << "---Demand on AP " << ap->getID() << ": " << ap->getCurrentDemand() << endl;

		/*add trip to bus*/
		currentBus->addTrip(selectedTrip);
		//cout << "Adding trip (D" << depot->getID()
		//		<< ",AP" << ap->getID()<< ") to bus..." << endl;

		/*get people on the bus (reduce pending demand on assembly point) and check if there are more people
		 * left to evacuate, for the next buses and iterations*/
		int satisfiedDemand = ap->reducePendingDemand(currentBus->getCapacity());
		if(satisfiedDemand == 0)
		{
			//There are no more people on the Assembly Point, remove the trips that conect this AP with
			//any Depot
			for(int i=0;i<problemInstance->getDepotList().size(); i++)
			{
				//remove all trips with endpoint = current AP
				problemInstance->getDepot(i)->removeAvailabletripByEndPoint(ap);
			}

			for(int i=0;i<problemInstance->getShelterList().size(); i++)
			{
				//remove all trips with startPoint = current AP
				problemInstance->getShelter(i)->removeAvailabletripByStartPoint(ap);
			}
		}
		//problemInstance->

		//cout << "---Updated demand on AP " << ap->getID() << ": " << ap->getCurrentDemand() << endl;


		//cout << "-----\n" << endl;

	}
	//cout << "-------------------------------------------------------------------------\n\n";
}

void Solver::localSearch() {

	/*HILL CLIMBING SOME IMPROVEMENT*/

	int it=1;

	while(it <= this->iterHCLimit)
	{
		//cout << "Current HC Iteration: " << it<< endl;
		if(!this->findBetterNeighbor()) //didn't find any neighbor better than current best solution.
			break;

		++it; // it= it+1 -> it>bla -- it>bla -> it=it+1
	}
}

bool Solver::findBetterNeighbor()
{
	//1. Generate neighbor: applying shift of trip (remove trip from a bus, insert it to another bus)
	//2. Check if neighbor is better than current best solution -> if better return true;
	//3. if doesn't improve -> unapply current neighbor


	float oldOFValue = currentSolution->getObjFuncValue();

	Bus * candidateRecipientBus;
	Bus * largestRouteBus = currentSolution->getLargestRouteBus();


	/*checks if the bus has at least 3 trips (one initial trip, two evacuation trips), to shift one of the evacuation trips*/
	if(largestRouteBus->getTripList().size() > 2)
	{
		/*assumes that all other buses are used for the shift*/

		for(int tripPosition=1 ;tripPosition<largestRouteBus->getTripList().size();tripPosition++)//for each trip made by bus with largest route
		{
			Trip* movingTrip = largestRouteBus->getTrip(tripPosition);

			//cout << "Moving trip: "<< movingTrip->getID()<< endl;
			//cout << "Removing trip from best solution in position: " << i << endl;


			if(largestRouteBus->removeTripInPosition(tripPosition))
			{

				/*for each candidate recipient bus (every bus that isn't the largest route bus)*/
				for(int j=0;j<currentSolution->getBusList().size();j++)
				{

					candidateRecipientBus = &(currentSolution->getBus(j));

					if(candidateRecipientBus->getID() != largestRouteBus->getID())
					{
						//cout << "Candidate bus:"<< candidateBus->getID() << endl;

						/*for each position in the bus schedule*/
						for(int k=1; k < candidateRecipientBus->getTripList().size() ;k++)
						{
							//cout << "Inserting trip: "<<movingTrip->getID() << " in position: "<< k << endl;
							if(candidateRecipientBus->addTripInPosition(movingTrip, k))
							{
								if(currentSolution->calculateOF() < oldOFValue)
								{
									//cout<< "Neighbor with improvement found!, old OF:"<< oldOFValue << ", new OF: "<< currentSolution->getObjFuncValue()<< endl;


									//UNCHECK TO TEST FEASIBILITY OF THE SOLUTION (ONLY FOR TESTING DUE TO PERFORMANCE)
									//if(currentSolution->checkFeasibility(problemInstance,false,false)){
										if(currentSolution->getObjFuncValue() < this->bestSolution.getObjFuncValue())
										{
											this->bestSolution = *currentSolution; //Update best solution
											this->bestSolution.timeFound = (double)(clock() - this->startTime)/CLOCKS_PER_SEC;
											//cout << "Best solution found at: " << this->bestSolution.timeFound << " seconds - Value: " << this->bestSolution.getObjFuncValue() << endl;
											//currentSolution->print();
										}
									//}else cout << "Solucion infactible\n\n"<< endl;



									return true; //neighbor with improvement found.
								}
								else
								{
									//cout << "didnt improve OF function, going to next position"<< endl;
									candidateRecipientBus->removeTripInPosition(k);
								}
							}
						}
					}
				}
				//at this point no neighbor with improvement has been found.
				largestRouteBus->addTripInPosition(movingTrip, tripPosition);
				currentSolution->calculateOF();
			}//endif largestRouteBus->removeTripInPosition(tripPosition)

		}//end for trips
	}

	return false;

}



void Solver::setCurrentBestSolution() {

	this->bestSolution.reset();

	for(Bus * current:problemInstance->getBusList())
	{
		this->bestSolution.addBusToSolution(*current);
	}

	this->bestSolution.setOF();
}

Solution * Solver::getSolutionFromProblemInstance()
{
	Solution * sol = new Solution();
	for(Bus * current:problemInstance->getBusList())
	{
		sol->addBusToSolution(*current);
	}
	sol->calculateOF();
	return sol;
}
