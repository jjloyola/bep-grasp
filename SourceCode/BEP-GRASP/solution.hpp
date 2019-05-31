#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include "dataStructures.hpp"
#include "tripMapping.hpp"
#include "problemInstance.hpp"
#include <limits>

class Solution
{
	vector<Bus> busList;
	float objFuncValue;
	Bus * largestRouteBus;
public:
	float avgTimeToGetOnABus, avgTimeToGetOnAShelter;
	double timeFound;
	Solution():objFuncValue(999999999){};
	Solution(vector<Bus> busList_,float objFuncValue_,Bus * largest):busList(busList_),objFuncValue(objFuncValue_),largestRouteBus(largest){}
	Bus & getBus(int busID){return this->busList[busID];}
	vector<Bus> getBusList(){return this->busList;}
	float getObjFuncValue(){return this->objFuncValue;}
	float calculateOF();
	float calculateOFSorting();
	bool checkFeasibility(ProblemInstance * problemInstance,bool printSucess=false, bool printFailure=false);

	void sortBusesByID();

	/*Calculate Objective Function Value*/
	void print();
	void addBusToSolution(Bus newBus){this->busList.push_back(newBus);};

	void reset(){busList.clear(); objFuncValue=0;};
	void setOF(){objFuncValue=this->calculateOF();}
	Bus * getLargestRouteBus(){ return this->largestRouteBus;}

	float getLargestRouteCost();
	float getShortestRouteCost();
	void calculateAvgTimes(float speedInKmMin, int instanceTotalDemand);
};

#endif /* SOLUTION_HPP_ */
