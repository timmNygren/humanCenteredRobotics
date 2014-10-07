#include "point2d.h"

Point2d::Point2d(double newX, double newY)
{
	x = newX;
	y = newY;
}

double Point2d::distance (const Point2d p) const
{
	return sqrt(pow((x-p.x),2) + pow((y-p.y),2));
}

double Point2d::angle (const Point2d p1) const
{
	double xDiff = p1.x - x;
	double yDiff = p1.y - y;
	double angle = atan2(yDiff, xDiff) * (180 / PI);
	return angle;
}

double Point2d::getDimension(const int dimension) const
{
	std::cout << "Point2d::getDimension Not implemented yet\n";
	return 0.0;
}

std::ostream& operator<<(std::ostream& out, const Point2d& p)
{
    return out << "xpos " << p.x << " ypos " << p.y;
}