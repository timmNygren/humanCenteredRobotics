#include "bin.h"

Bin::Bin(double lowBound, double interval)
{
	lowerBound = lowBound;
	upperBound = lowBound + interval;
	numInBin = 0;
	freq = 0.0; 
}

void Bin::addToBin(double val)
{
	numInBin += val;
}

void Bin::normalize(double size)
{
	double newFreq = (double)numInBin / size;
	freq = newFreq;
}

bool Bin::valueInBin(double data)
{
	// If the data fits in the bin, return true
	if (data >= lowerBound && data < upperBound)
		return true;
	else
		return false;
}

double Bin::getFreq()
{
	return freq;
}

double Bin::getBinSize()
{
	return numInBin;
}

std::ostream& operator<<(std::ostream& out, const Bin& b)
{
	// Debug
	return out << "Low bound: " << b.lowerBound << " Upper bound: " << b.upperBound << " Bin size: " << b.numInBin << " Normalized Freq: " << b.freq;
}