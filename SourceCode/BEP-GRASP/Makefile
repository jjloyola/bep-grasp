CC= g++
CFLAGS= -std=c++11 -O3
VARFLAGS=
#CFLAGS= -std=c++11 -O3 -fmax-errors=2 -ggdb #for Debug
#CXXFLAGS =	-O2 -g# -Wall -fmax-errors=2 -fmessage-length=0
#DEPS= Utilities.hpp dataStructures.hpp problemInstance.hpp instanceReader.hpp tripMapping.hpp solver.hpp solution.hpp#headers.h 
OBJS = main.o fileHandler.o problemInstance.o dataStructures.o tripMapping.o solver.o solution.o utils.o
TARGETSIMPLE = bep-grasp

#TARGETCPLEXCOMP = bepGRASP-cplexComp #--uncomment to run the comparison with cplex results
#TARGETQUALITYEVOL = bepGRASP-qualityEvol #--uncomment to check change of quality of the solution over time

.PHONY: all
all:
	$(MAKE) bep
	

bep: $(TARGETSIMPLE)

cplexComp: $(TARGETCPLEXCOMP)

qualityEvol: $(TARGETQUALITYEVOL)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(VARFLAGS)

$(TARGETSIMPLE) $(TARGETCPLEXCOMP) $(TARGETQUALITYEVOL): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean cleanobj
clean : cleanobj
	rm -f $(TARGETSIMPLE) $(TARGETCPLEXCOMP) $(TARGETQUALITYEVOL)

cleanobj :
	rm -f *.o


#target specific overrides
#cplexComp: VARFLAGS += -DCPLEX_COMP=1 -DSTOP_WHEN_REACHED=1
#qualityEvol: VARFLAGS += -DSAVE_QUALITY_EVOLUTION=1


ex-rand: all
	./bep-grasp -i instances/rand/set4/BEP-1-4-4-3-i1.txt -s 5 -ref 0.5 -ithc 80 -t 3 -o BEP-1-4-3-3-i1-solution.txt; spd-say done

ex-real: all
	./bep-grasp -i instances/real/BEP-5-52-12-50.txt -s 7 -ref 0.1 -ithc 80 -t 3 -o BEP-5-52-12-50-solution.txt; spd-say done
