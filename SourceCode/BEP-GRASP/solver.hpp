#ifndef SOLVER_HPP_
#define SOLVER_HPP_
#include "fileHandler.hpp"
#include "problemInstance.hpp"
#include "solution.hpp"

class Solver
{
	float refListProportion;
	Solution bestSolution;
	time_t startTime;
	time_t timeLimit;
	int iterHCLimit;
	Solution *currentSolution;

public:
	double timeToReachCplex;
	ProblemInstance * problemInstance;
	Solver(ProblemInstance * newInstance,float prop,time_t startTime_,int timeLimit_, int iterHCLimit_):
		problemInstance(newInstance),refListProportion(prop),startTime(startTime_),timeLimit(timeLimit_),iterHCLimit(iterHCLimit_),timeToReachCplex(-1.0f)
		{bestSolution=Solution();currentSolution=NULL;}
	Solution grasp(vector<string> * qualityEvolution=NULL);
	Solution graspCplexComp(float cplexResult);
	void greedyRandomizedConstructor();
	void assignDepoApTrip();
	void localSearch();
	bool findBetterNeighbor();
	Solution getBestSolution(){return this->bestSolution;}
	void setCurrentBestSolution();

	Solution * getSolutionFromProblemInstance();


	void addToCSV();

	//map<float,Trip*> getReferenceList(LocationNode * node,int size);
};




#endif /* SOLVER_HPP_ */
