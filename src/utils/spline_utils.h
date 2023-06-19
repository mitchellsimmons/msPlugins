// Contains a set of functions for computing spline based operations (eg. polynomials, parametric equations)
// Functions are mostly static and have been placed into classes for organisational purposes

#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>

#include "math_utils.h"
#include "matrix_utils.h"
#include "quaternion_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Base class which implements general curve functionality
class Spline
{
public:
	// ------ Curvature ------
	static double approximateCurvature(const MVector& vTangentPrevious, const MVector& vTangentCurrent, double tDelta);

	// ------ Parameterization ------
	static void computeNaturalParameters(std::vector<double>& outParameters, double lowerBound = 0.0, double upperBound = 1.0);

	static void computeArcLengthParameters(const std::vector<double>& lengths, std::vector<double>& outParameters);

	static double arcLengthToNaturalParameter(double arcLengthParameter, const std::vector<double>& lengths);

	static double naturalToArcLengthParameter(double naturalParameter, const std::vector<double>& lengths);
	
	// ------ RMF ------
	static MVector computeProjectedNormalRMF(const MVector& vNormalPrevious, const MVector& vTangentCurrent);

	static MQuaternion computeDoubleReflectionRMF(const MVector& vPointPrevious, const MVector& vPointCurrent, 
		const MVector& vTangentPrevious, const MVector& vTangentCurrent);

protected:
	Spline();
	~Spline();
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Derived class for non-rational Bezier curves of any degree (all basis weights are equal to 1)
class Bezier : public Spline
{
public:
	Bezier();
	~Bezier();

	// ------ Parameterization ------
	static void computeLengths(const std::vector<MVector>& controlPoints, std::vector<double>& outlengths, std::vector<MVector>* outPoints = nullptr);

	// ------ Sample ------
	static double sampleBasis(double x, unsigned int degree, unsigned int k);

	static MVector sampleCurve(double t, const std::vector<MVector>& controlPoints);

	static MVector sampleDerivative(unsigned int order, double t, const std::vector<MVector>& controlPoints);

	static double sampleCurvature(double t, const std::vector<MVector>& controlPoints);
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Defines a cubic trigonemetric Bezier which is defined by four control points and incorporates two shaping parameters
// The class itself does not derive from a Bezier as it implements its own sampling functions which account for the shaping parameters
// The shaping parameters provide the user with extra local control over the shape of the curve
// - When the shaping parameters are set to a minimum, the curve takes on the form of a standard cubic Bezier
// - As the influence increases towards a maximum, the curve's shape becomes more influenced by its control polygon (hull)
class CubicTBezier : public Spline
{
public:
	CubicTBezier();
	~CubicTBezier();

	// ------ Parameterization ------
	static void computeLengths(double shape1, double shape2, const std::vector<MVector>& controlPoints, 
		std::vector<double>& outlengths, std::vector<MVector>* outPoints = nullptr);

	static void computeSplitLengthParameters(double shape1, double shape2, const std::vector<MVector>& controlPoints,
		const std::vector<MVector>& points, const std::vector<double>& lengths, std::vector<double>& outParameters);

	static double splitLengthToNaturalParameter(double splitLengthParameter, double shape1, double shape2,
		const std::vector<MVector>& controlPoints, const std::vector<MVector>& points, const std::vector<double>& lengths);

	// ------ Sample ------
	static MVector sampleCurve(double t, double shape1, double shape2, const std::vector<MVector>& controlPoints);

	static MVector sampleFirstDerivative(double t, double shape1, double shape2, const std::vector<MVector>& controlPoints);
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Non-rational basis spline (essentially a simplified nurbs where all basis weights are equal to 1)
class BSpline : public Spline
{
public:
	BSpline();
	~BSpline();

	// ------ Parameterization ------
	static void computeLengths(unsigned int degree, const std::vector<double>& knots, const std::vector<MVector>& controlPoints,
		std::vector<double>& outlengths, std::vector<MVector>* outPoints = nullptr);

	// ------ Knots ------
	static std::vector<double> computeClampedKnotVector(unsigned int n, unsigned int degree);

	static std::vector<double> computeUnclampedKnotVector(unsigned int n, unsigned int degree);

	static unsigned int getKnotInterval(double t, unsigned int degree, const std::vector<double>& knots);
	
	// ------ Sample ------
	static MVector sampleCurve_DEPRECATED(double t, unsigned int degree,
		const std::vector<double>& knots, const std::vector<MVector>& controlPoints);

	static MVector sampleCurve(double t, unsigned int degree,
		const std::vector<double>& knots, const std::vector<MVector>& controlPoints);

	static MVector sampleDerivative(unsigned int order, double t, unsigned int degree,
		const std::vector<double>& knots, const std::vector<MVector>& controlPoints);

	static double sampleCurvature(double t, unsigned int degree,
		const std::vector<double>& knots, const std::vector<MVector>& controlPoints);
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------