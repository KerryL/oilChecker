// File:  tankGeometry.h
// Date:  12/22/2020
// Auth:  K. Loux
// Desc:  Oil tank geometry calculations.

#ifndef TANK_GEOMETRY_H_
#define TANK_GEOMETRY_H_

// Local headers
#include "oilCheckerConfig.h"

class TankGeometry
{
public:
	virtual double ComputeRemainingVolume(const double& measuredDistance) const = 0;// [gal]
};

class VerticalTankGeometry : public TankGeometry
{
public:
	VerticalTankGeometry(const TankDimensions& dimensions) : dimensions(dimensions) {}

	double ComputeRemainingVolume(const double& measuredDistance) const override;// [gal]

private:
	TankDimensions dimensions;

	static double CircularSegmentArea(const double& radius, const double distance);
};

#endif// TANK_GEOMETRY_H_
