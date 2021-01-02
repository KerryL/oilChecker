// File:  tankGeometry.cpp
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil tank geometry calculations.

// Local headers
#include "tankGeometry.h"

// Standard C++ headers
#include <cmath>
#include <cassert>

double VerticalTankGeometry::ComputeRemainingVolume(const double& measuredDistance) const
{
	const double level(dimensions.height - measuredDistance + dimensions.heightOffset);
	const double radius(0.5 * dimensions.width);
	const double halfCircleArea(0.5 * M_PI * radius * radius);
	double areaSqInch(0.0);
	if (level > dimensions.height - radius)// Level in top half circle
	{
		areaSqInch = halfCircleArea;// Bottom half circle
		areaSqInch += dimensions.width * (dimensions.height - dimensions.width);// Center rectangle
		areaSqInch += halfCircleArea - CircularSegmentArea(radius, level - dimensions.height - radius);// Portion of top half circle
	}
	else if (level > radius)// Level in central rectangle
	{
		areaSqInch = halfCircleArea;// Bottom half circle
		areaSqInch += dimensions.width * (level - radius);// Portion of center rectangle
	}
	else// Level in bottom half circle
	{
		const double levelBelowHalfCircle(radius - level);
		assert(levelBelowHalfCircle > 0.0);
		areaSqInch = CircularSegmentArea(radius, levelBelowHalfCircle);// Portion of bottom half circle
	}

	return areaSqInch * dimensions.length * 0.004329;// [gal]
}

// Computes the area bounded by a circle and a line offset a distance from the center of the circle
double VerticalTankGeometry::CircularSegmentArea(const double& radius, const double distance)
{
	const double dOverR(distance / radius);
	return radius * (radius * acos(dOverR) - distance * sqrt(1.0 - dOverR * dOverR));
}
