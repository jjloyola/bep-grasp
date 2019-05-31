#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h> /* fabs */


namespace utils
{
	void saveToFile(const std::string & filename, const std::string & text, bool coutScreen)
	{
		std::fstream fs;
		fs.open (filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
		fs << text<<std::endl;
		fs.close();
		if(coutScreen)
			std::cout<<text<<std::endl;
	}

	void saveToFile(const std::string &filename,const std::string &text, bool saltoLinea, bool coutScreen)
	{
		std::fstream fs;
			fs.open (filename.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
			fs << text;
			if(saltoLinea)
				fs<<std::endl;
			fs.close();
			if(coutScreen)
				std::cout<<text;
				if(saltoLinea)
					std::cout << std::endl;
	}


	bool float_eq(float A, float B, float tolerance)
	{
		/*Compare two floats with certain tolerance (0.005f default).
		 * Returns: true if both are equal, false otherwise. */
		return (fabs(A - B) < tolerance);
	}

	bool float_leq(float A, float B, float tolerance)
	{
		/*Compare two floats with certain tolerance (0.005f default).
		 * Returns: true if A is lesser or equal to B, false otherwise. */
		return (fabs(A - B) < tolerance) || (A < B);
	}

	bool float_geq(float A, float B, float tolerance)
	{
		/*Compare two floats with certain tolerance (0.005f default).
		 * Returns: true if A is greater or equal to B, false otherwise. */
		return (fabs(A - B) > tolerance) || (A < B);
	}

	void SystemPause()
	{
		std::cout << "Press ENTER to continue... " << flush;
		std::cin.ignore( std::numeric_limits <std::streamsize> ::max(), '\n' );
	}

	float float_round(float number, int decimals)
	{
		return roundf(number * (10*decimals)) / (10*decimals);
	}

	bool double_eq(double A, double B, float tolerance)
	{
		/*Compare two floats with certain tolerance (0.005f default).
		 * Returns: true if both are equal, false otherwise. */
		return (fabs(A - B) < tolerance);
	}

	bool double_leq(double A, double B, float tolerance)
	{
		/*Compare two floats with certain tolerance (0.005f default).
		 * Returns: true if both are equal, false otherwise. */
		return (fabs(A - B) < tolerance) || (A < B);
	}
}

