#pragma once

#include <iostream>

class Bin 
{
	public:
	double lowerBound;
	double upperBound;
	double numInBin;
	double freq;

	Bin(double lowBound, double interval);
	void addToBin(double val = 1.0);
	void normalize(double size);
	bool valueInBin(double data);
	double getFreq();
	double getBinSize();
};

std::ostream& operator<<(std::ostream& out, const Bin& b);