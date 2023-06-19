// Contains a general set of functions for general mathematical operations

#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <unordered_map>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Interpolation ------

enum Easing : short
{
	kLinear = 0,
	kInSine = 1,
	kInQuad = 2,
	kInCubic = 3,
	kInQuart = 4,
	kInQuint = 5,
	kInExpo = 6,
	kInCirc = 7,
	kOutSine = 8,
	kOutQuad = 9,
	kOutCubic = 10,
	kOutQuart = 11,
	kOutQuint = 12,
	kOutExpo = 13,
	kOutCirc = 14,
	kInOutSine = 15,
	kInOutQuad = 16,
	kInOutCubic = 17,
	kInOutQuart = 18,
	kInOutQuint = 19,
	kInOutExpo = 20,
	kInOutCirc = 21,
};

double lerp(double a, double b, double t);

double smoothstep(double a, double b, double t);

double variableSmoothstep(double a, double b, double t, double s);

double variableAcceleration(double a, double b, double t, double s);

double variableDeceleration(double a, double b, double t, double s);

double inSineInterp(double a, double b, double t);

double inQuadInterp(double a, double b, double t);

double inCubicInterp(double a, double b, double t);

double inQuartInterp(double a, double b, double t);

double inQuintInterp(double a, double b, double t);

double inExpoInterp(double a, double b, double t);

double inCircInterp(double a, double b, double t);

double outSineInterp(double a, double b, double t);

double outQuadInterp(double a, double b, double t);

double outCubicInterp(double a, double b, double t);

double outQuartInterp(double a, double b, double t);

double outQuintInterp(double a, double b, double t);

double outExpoInterp(double a, double b, double t);

double outCircInterp(double a, double b, double t);

double inOutSineInterp(double a, double b, double t);

double inOutQuadInterp(double a, double b, double t);

double inOutCubicInterp(double a, double b, double t);

double inOutQuartInterp(double a, double b, double t);

double inOutQuintInterp(double a, double b, double t);

double inOutExpoInterp(double a, double b, double t);

double inOutCircInterp(double a, double b, double t);

// An easing function can be retrieved from the map as follows:
// double (*easingFunc)(double, double, double) = MRS::easingFuncMap.find(easing)->second;
static const std::unordered_map<Easing, double(*)(double, double, double)> easingFuncMap = {
	{kLinear, &lerp}, {kInSine, &inSineInterp}, {kInQuad, &inQuadInterp}, {kInCubic, &inCubicInterp},
	{kInQuart, &inQuartInterp}, {kInQuint, &inQuintInterp}, {kInExpo, &inExpoInterp}, {kInCirc, &inCircInterp},
	{kOutSine, &outSineInterp}, {kOutQuad, &outQuadInterp}, {kOutCubic, &outCubicInterp},
	{kOutQuart, &outQuartInterp}, {kOutQuint, &outQuintInterp}, {kOutExpo, &outExpoInterp}, {kOutCirc, &outCircInterp},
	{kInOutSine, &inOutSineInterp}, {kInOutQuad, &inOutQuadInterp}, {kInOutCubic, &inOutCubicInterp},
	{kInOutQuart, &inOutQuartInterp}, {kInOutQuint, &inOutQuintInterp}, {kInOutExpo, &inOutExpoInterp}, {kInOutCirc, &inOutCircInterp},
};

// ------ Misc ------

bool isEqual(double a, double b);

bool isEqualWithin(double a, double b, double epsilon = 0.0001);

template <typename TType>
TType clamp(TType v, TType low, TType high)
{
	return std::max(low, std::min(v, high));
}

double remap(double v, double low1, double high1, double low2, double high2);

int remap(int v, int low1, int high1, int low2, int high2);

int pow(int x, unsigned int exp);

unsigned int factorial(unsigned int n);

unsigned int permutations(unsigned int n, unsigned int k);

unsigned int binomialCoefficient(unsigned int n, unsigned int k);

int forwardDifferenceCoefficient(unsigned int n, unsigned int k);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------