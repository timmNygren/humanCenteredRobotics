#include "point3d.h"

Point3d::Point3d(double newX, double newY, double newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

double Point3d::distance(const Point3d p) const
{
	return sqrt(pow((x-p.x),2) + pow((y-p.y),2) + pow((z-p.z),2));
}

double Point3d::angle (const Point3d p1, const Point3d p2) const
{
	// Create two vectors from three points
	Point3d vector1 = Point3d(p1.x - x, p1.y - y, p1.z - z);
	Point3d vector2 = Point3d(p2.x - x, p2.y - y, p2.z - z);
	// calculate the magnitude and normal vectors
	double v1mag = sqrt( pow(vector1.x,2) + pow(vector1.y,2) + pow(vector1.z,2) );
	Point3d v1norm = Point3d(vector1.x / v1mag, vector1.y / v1mag, vector1.z / v1mag);

	double v2mag = sqrt( pow(vector2.x,2) + pow(vector2.y,2) + pow(vector2.z,2) );
	Point3d v2norm = Point3d(vector2.x / v2mag, vector2.y / v2mag, vector2.z / v2mag);
	// calculate the dot product of the norms
	double dot = v1norm.x * v2norm.x + v1norm.y * v2norm.y + v1norm.z * v2norm.z;
	// return the angle
	return acos(dot);
}

Point3d Point3d::difference(const Point3d p) const
{
	return Point3d(p.x - x, p.y - y, p.z - z);
}

double Point3d::getDimension(const int dimension) const
{
	if (dimension == 0)
		return x;
	else if (dimension == 1)
		return y;
	else
		return z;
}

std::ostream& operator<<(std::ostream& out, const Point3d& p)
{
	// Debug
    return out << "xpos " << p.x << " ypos " << p.y << " zpos " << p.z;
}