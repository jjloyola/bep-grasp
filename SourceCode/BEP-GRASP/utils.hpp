#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <string>
#include <ostream>
#include <limits>
#include "dataStructures.hpp"


namespace utils
{
	void saveToFile(const std::string & fileName,const std::string & text, bool coutScreen);
	void saveToFile(const std::string & fileName,const std::string & text,bool saltoLinea, bool coutScreen);
	bool float_eq(float A, float B, float tolerance = 0.001f);
	bool float_leq(float A, float B, float tolerance = 0.001f);
	bool float_geq(float A, float B, float tolerance = 0.001f);
	float float_round(float number, int decimals);
	bool double_eq(double A, double B, float tolerance = 0.001f);
	bool double_leq(double A, double B, float tolerance = 0.001f);
	void SystemPause();
}




#endif
