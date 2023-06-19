#pragma once

#include <vector>

#include <maya/MVector.h>
#include <maya/MAngle.h>

#include <SeExpr2/Curve.h>
#include <SeExpr2/Vec.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Custom data structures to store local adjustment values
struct Adjustment
{
	std::vector<SeExpr2::Curve<double>::CV> controlPoints;
	SeExpr2::Curve<double> curve;
	double offset;
	double falloffDistance;
	short falloffMode;
	bool repeat;
};

struct PositionAdjustment : public Adjustment
{
	MVector vPosition;
};

struct TwistAdjustment : public Adjustment
{
	MAngle twist;
};

struct ScaleAdjustment : public Adjustment
{
	MVector vScale;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------