#pragma once

#include <iostream>
#include <cmath>

#define PI 3.14159265

class Point2d
{
	public:
	double x, y;

	Point2d(double newX, double newY);
	double distance (const Point2d p) const;
	double angle (const Point2d p1) const;
	double getDimension(const int dimension) const;
};

std::ostream& operator<<(std::ostream& out, const Point2d& p);