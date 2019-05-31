/*********************************************
 * OPL 12.6.0.0 Model
 * Author: Javiera Loyola
 * Creation Date: 15-10-2016 at 18:04:07
 *********************************************/

//------------*Problem Size*----------------

int D=...;//number of depots (read from dat file)
int P=...;//number of assembly points (read from dat file)
int S=...;//number of shelters (read from dat file)
int B=...;//number of buses (read from dat file)


//----------------------------------------------------------------------------------
//------*Variables to represent the problem, and use them on the model*-------------

range apoints=1..P; //indexes for apoints
range shelters=1..S; //indexes for shelters
range buses=1..B; //indexes for buses

//create a set of trips, which contains a combination of all apoints and all shelters
tuple trip {
	int i; //startingpoint
	int j; //endingpoint
}
setof(trip) trips = {<i,j> | i in apoints, j in shelters};


float dist_start[apoints] = ...;
float dist[trips] = ...;
int demands[apoints]=...;
int capacities[shelters]=...;


int R = sum (ap in apoints) demands[ap];
range rounds=1..R;
range rounds2 = 1..R-1;

tuple bus_round {
	int b;
	int r;
}

setof(bus_round) bus_rounds = {<b,r> | b in buses,r in rounds};


//time limit: 15 min (900 seg)
execute PARAMS{
	cplex.tilim = 10;
	//cplex.tilim = 900;
}


//------------------------------------------------------
//-------------* Mathematical Model *-------------------


//variables of the model
dvar boolean x[bus_rounds][trips];
dvar float+ t_to[bus_rounds];
dvar float+ t_back[bus_rounds];
dvar float+ t_max;

minimize t_max;

subject to{
	 
	//constraint 3
	forall (b in buses,r in rounds)
	    measured_travel_time_to:
	    sum (i in apoints,j in shelters) dist[<i,j>]*x[<b,r>][<i,j>] == t_to[<b,r>];
	 
	//constraint 4
	forall(b in buses,r in rounds2,i in apoints,j in shelters)	  
		measured_travel_time_back:
 		dist[<i,j>]*( (sum (k in apoints) x[<b,r>][<k,j>]) + (sum (l in shelters) (x[<b,r+1>][<i,l>])) - 1 ) <= t_back[<b,r>];
	
	//constraint 2
	forall (b in buses)
	  max_evac_time:
	  sum (r in rounds) (t_to[<b,r>]+t_back[<b,r>]) + sum (tr in trips) dist_start[tr.i]*x[<b,1>][tr] <= t_max;
	
	//constraint 5
	forall (br in bus_rounds)
	  one_trip_round:
	  sum (tr in trips) x[br][tr] <=1;
	
	//contraint 6
	forall (b in buses,r in rounds2)
	    stop_anytime:
	    sum (tr in trips) x[<b,r>][tr] >= sum (tr in trips) x[<b,r+1>][tr];
	
	//constraint 7
	forall(i in apoints)
	  all_evacuated:
	  sum (j in shelters, br in bus_rounds) x[br][<i,j>] >= demands[i];
	
	//constraint 8
	forall(j in shelters)
	  capacities_respected:
	  sum (i in apoints, br in bus_rounds) x[br][<i,j>] <= capacities[j];
}



//------------------------------------------------
//----------* Control of the process*-------------

main
{
	thisOplModel.settings.mainEndEnabled = true;
	var ofile = new IloOplOutputFile();
	
	//archivo en que se van a escribir los resultados
   	ofile.open("bep_Cplex_results.txt");
	ofile.writeln("set;instance;solution;time;cplexStatus;");

	for(var setID=2;setID<=2;setID++) //ejecutar solo set 2
	{
		for(var instID=1;instID<=1;instID++) //ejecutar solo instancia 2
		{
			writeln("Solving Set " + setID + " Instance " + instID + "..."); //write in console
			var model = thisOplModel.modelDefinition;
			var data = new IloOplDataSource("data_s" + setID + "i" + instID + ".dat"); //define data source
			
			var MyCplex = new IloCplex();
			var opl = new IloOplModel(model,MyCplex);
			opl.addDataSource(data);
			opl.generate();
			
			//write results on file
			if(MyCplex.solve()){
				ofile.writeln(setID,";",
					instID,";",
					MyCplex.getObjValue(),";",
					MyCplex.getDetTime(),";",
					MyCplex.getCplexStatus(),";");
			}
			else
			{
				ofile.writeln(setID,";",
					instID,";",
					MyCplex.getBestObjValue(),";",
					MyCplex.getDetTime(),";",
					MyCplex.getCplexStatus(),";");
 			}


			opl.end();
			MyCplex.end();
		}//end for instID
	}//end for set
	
	ofile.close();
	writeln("Done!"); //write in console
}