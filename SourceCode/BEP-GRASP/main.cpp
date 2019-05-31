#include <stdio.h>
#include <stdlib.h>
#include "problemInstance.hpp"
#include "utils.hpp"
#include "dataStructures.hpp"
#include "solver.hpp"
#include "solution.hpp"
#include <time.h>
#include "fileHandler.hpp"
#include "mainHeader.hpp"
#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

void handleParameters( int argc, char** argv,string & inputFilename,  string & outputFilename,
		int & seed, time_t &timeLimit, int &numberOfIterations, int & maxHCIterations, float & referenceListPercentage, bool & saveQualityEvol);

#if CPLEX_COMP==1
double getCplexResult(string instanceName);
#endif

int main(int argc, char* argv[])
{
	std::string inputFilename;
	std::string outputFilename="";
	int seed,
		maxIterations=0,
		maxHCIterations=0;
	float refListSize=0.3,
		speedInKmMin=1;//Speed of the buses in Km per Min

	time_t timeLimit=180; //time limit in seconds
	double cplexResult = -1;


	bool saveQualityEvol = false; //save time vs OFvalue for graph


	if(argc==1)
	{
		//./bep -i instances/real/InstanceBEP-5-52-12-125.txt -t 5 -s 5 -ref 0.3 -ithc 20 -o RESULT-InstanceBEP-5-52-12-125.txt

		inputFilename="instances/real/InstanceBEP-5-52-12-50.txt";
		outputFilename="instances/real/InstanceBEP-5-52-12-50-SOLUTION.txt";
		seed = 5;
		timeLimit=5;
		maxIterations=10;
		maxHCIterations=10;
		//cout << "Using default values\nFilename: " << inputFilename << "\nSeed: " << seed << "\nRefListSize: "<< refListSize <<"\n\n";
	}
	else
		handleParameters(argc,argv,inputFilename, outputFilename, seed, timeLimit, maxIterations,maxHCIterations, refListSize, saveQualityEvol);

	srand (seed);

	FileHandler reader;
	ProblemInstance * instance = reader.readFile(inputFilename);
	vector<string> qualityEvolVSTime;

	if(saveQualityEvol) qualityEvolVSTime = {"Time[s];BestOF"};

#if CPLEX_COMP==1
	cplexResult = getCplexResult(inputFilename);
#endif




	//--------------START OF THE ALGORITHM----------------------------------

	timeLimit = timeLimit*(double)CLOCKS_PER_SEC;
	time_t startTime = clock();

	Solver * solver = new Solver(instance,refListSize,startTime,timeLimit,maxHCIterations); //send maxHCIterations

	Solution best;
#if CPLEX_COMP==0
		best = solver->grasp(&qualityEvolVSTime);
#else
		best = solver->graspCplexComp(cplexResult);
#endif
	//--------------END OF THE ALGORITHM----------------------------------


	//best.print();

	//calculate Diff statistics
	float diff = best.getLargestRouteCost()-best.getShortestRouteCost(); //difference between route with min and max cost in a solution
	best.calculateAvgTimes(speedInKmMin, instance->getTotalDemand()); //60 kph, UPDATE


	//cout << "Best FO: " << best.getObjFuncValue() << endl;

	if(outputFilename != "")
		reader.saveSolutionToFile(outputFilename, &best);

	inputFilename = reader.removePathFromFilename(inputFilename);
	inputFilename = reader.removeExtensionFromFilename(inputFilename);




	//Adding values to vector, to insert to csv
	vector<string> toCSV;

	//HEAD: Input;Seed;ReferenceListPercentage;MaxHCIterations;TimeLimit[s];BestFO;TimeFound;\
	Cplex BestFO 1HR; Time used to reach CPLEX [ms];Diff Distance[Km];average time to get on bus [min];avg time to get on shelter [min]

	toCSV.push_back(inputFilename);
	toCSV.push_back(to_string(seed)); //-s
	toCSV.push_back(to_string(refListSize)); //-ref
	toCSV.push_back(to_string(maxHCIterations)); //-ithc
	toCSV.push_back(to_string(timeLimit/(double)CLOCKS_PER_SEC)); //-t
	toCSV.push_back(to_string(best.getObjFuncValue()));
	toCSV.push_back(to_string(best.timeFound));//time when the best solution was found in miliseconds
#if CPLEX_COMP==1
		toCSV.push_back(to_string(cplexResult)); //cplex result with 1 hr execution
		toCSV.push_back(to_string(solver->timeToReachCplex)); //time that used the algorithm to reach the cplex value (random instances) in miliseconds
		toCSV.push_back(to_string( (best.getObjFuncValue()/cplexResult)-1.000f )); //proportion difference between grasp result and cplex result, number between 0 and 1
		toCSV.push_back(to_string(TOLERANCE));
#endif
	toCSV.push_back(to_string(diff));//difference between cost of largest route bus and shortest route bus
	toCSV.push_back(to_string(best.avgTimeToGetOnABus));//average time to get on a bus
	toCSV.push_back(to_string(best.avgTimeToGetOnAShelter));//average time to get on a shelter

	reader.csvAddLineVector(toCSV);

	//USE ONLY TO SAVE THE EVOLUTION OF THE QUALITY VS TIME, WITH INPUT PARAMETER -saveQualityEvol
	if(saveQualityEvol)
		reader.writeStringVectToFile("GRASP-QualityVSTime_s" + to_string(seed) + "_"+ inputFilename + ".txt", qualityEvolVSTime);
	//--------------------------------------------------------------------------------------------



	return EXIT_SUCCESS;
}


void handleParameters(int argc, char** argv,string & inputFilename, string & outputFilename, int & seed, time_t &timeLimit,
		int &numberOfIterations, int & maxHCIterations, float & referenceListPercentage, bool & saveQualityEvol)
{
	for(int i = 1; i < argc; i++)
	{
		if(string(argv[i])==string("-h") || string(argv[1])==string("-help"))
		{
			cout << "\n*Use the following abreviations for input parameters:\n\n"
					<<"-i \tinputFilename " << endl
					<<"-o \toutputFilename " << endl
					<<"-s \trandSeed \n-ref \trefListSize \n-t \ttimeLimit" << endl
					//<<"-it \tmaxIterations " << endl
					<<"-ithc \tmaxHCIterations\n\n"
					<<"*Example: ./bep -i instances/real/InstanceBEP-5-52-12-50.txt -s 5 -ref 0.3 -ithc 10 -t 10 -o result.txt\n\n"
					<<"---Note: If no abreviation is used, the algorithm will work with default parameters\n";
			exit(0);
		}
		if(string(argv[i]) == string("-i"))
			inputFilename = string(argv[i+1]);
		if(string(argv[i]) == string("-o"))
			outputFilename = string(argv[i+1]);
		if(string(argv[i]) == string("-s"))
			seed = atoi(argv[i+1]);
		if(string(argv[i]) == string("-t"))
			timeLimit = atoi(argv[i+1]);
		if(string(argv[i]) == string("-it"))
			numberOfIterations = atoi(argv[i+1]);
		if(string(argv[i]) == string("-ithc"))
			maxHCIterations = atoi(argv[i+1]);
		if(string(argv[i]) == string("-ref"))
			referenceListPercentage = atof(argv[i+1]);
		if(string(argv[i]) == string("-saveQualityEvol"))
			saveQualityEvol = true;
	}
}

#if CPLEX_COMP==1
double getCplexResult(string instanceName)
{
	vector<float> cplexResults (10);
	//Vector form = {i10,i1,i2,i3,i4,i5,i6,i7,i8,i9}
	if(instanceName.find("set2")!=string::npos) cplexResults = {38,46,13,9,44,28.9,44,18,16,12};
	else if(instanceName.find("set3")!=string::npos) cplexResults = {70,31,52,30,60,7.9,46,16,23.9,19.9};
	else if(instanceName.find("set4")!=string::npos) cplexResults = {23.9,23.9,30.9,24.9,40.9,12.9,15.9,20.9,19.9,17};
	else if(instanceName.find("set5")!=string::npos) cplexResults = {21.9,21.9,22.9,40.9,45.9,17.9,16.9,21.9,23.9,36.9};
	else if(instanceName.find("set6")!=string::npos) cplexResults = {21.9,22.9,14.9,21.9,24.9,10.9,23.9,15.9,20.9,25.9};
	else if(instanceName.find("set7")!=string::npos) cplexResults = {22.9,14.9,20.9,27.9,20.9,25,25.9,19.9,29,13.9};
	else if(instanceName.find("set8")!=string::npos) cplexResults = {19.9,22.9,20,16.9,18.9,27,26.9,20.9,23.9,22.9};
	else if(instanceName.find("set9")!=string::npos) cplexResults = {18,29.9,14.9,18.9,22.9,30.9,27.9,21,15.9,21.9};
	else if(instanceName.find("set10")!=string::npos) cplexResults = {25.9,16.9,28.9,30.9,20.9,15.9,42.9,21.9,14.9,26.9};

	if(instanceName.find("i10")!=string::npos) return cplexResults[0];
	else if(instanceName.find("i1")!=string::npos) return cplexResults[1];
	else if(instanceName.find("i2")!=string::npos) return cplexResults[2];
	else if(instanceName.find("i3")!=string::npos) return cplexResults[3];
	else if(instanceName.find("i4")!=string::npos) return cplexResults[4];
	else if(instanceName.find("i5")!=string::npos) return cplexResults[5];
	else if(instanceName.find("i6")!=string::npos) return cplexResults[6];
	else if(instanceName.find("i7")!=string::npos) return cplexResults[7];
	else if(instanceName.find("i8")!=string::npos) return cplexResults[8];
	else if(instanceName.find("i9")!=string::npos) return cplexResults[9];

	return -1;

}
#endif
