#pragma once

#include <iostream>
#include <cmath>

class Point3d
{
	public:
	double x,y,z;

	Point3d(double newX, double newY, double newZ);
	double distance (const Point3d p) const;
	double angle (const Point3d p1, const Point3d p2) const;
	Point3d difference(const Point3d p) const;
	double getDimension(const int dimension) const;
};

std::ostream& operator<<(std::ostream& out, const Point3d& p);