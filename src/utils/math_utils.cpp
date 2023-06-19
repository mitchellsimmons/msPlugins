#include "math_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Interpolation ------

/*	Linear interpolation over range [a, b], constant velocity
	Parameterized by t, with domain [0, 1]    */
double lerp(double a, double b, double t)
{
	assert(t >= 0.0 && t <= 1.0);

	return a + (b - a) * t;
}

/*	Sigmoidal interpolation over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double smoothstep(double a, double b, double t)
{
	assert(t >= 0.0 && t <= 1.0);

	t = t * t * (3 - 2 * t);
	return lerp(a, b, t);
}

/*	Defines a continuous family of sigmoidal interpolation functions whose shapes vary over a smoothing parameter s
	Each function interpolates over range [a, b], accelerating until halfway, then decelerating
	Each function is parameterized by t, with domain [0, 1]
	See - https://www.desmos.com/calculator/3zhzwbfrxd    */
double variableSmoothstep(double a, double b, double t, double s)
{
	assert(t >= 0.0 && t <= 1.0);
	assert(s >= 0.0 && s <= 1.0);

	// Limit the smoothing factor to half the defined range, otherwise the function increases too steepily around the center
	s *= 0.5;

	if (!isEqual(s, 0.0))
	{
		double c = 2.0 / (1.0 - s) - 1.0;

		if (t <= 0.5)
			t = std::pow(t, c) / std::pow(0.5, c - 1.0);
		else
			t = 1.0 - std::pow(1.0 - t, c) / std::pow(0.5, c - 1.0);
	}

	return lerp(a, b, t);
}

double variableAcceleration(double a, double b, double t, double s)
{
	// As s approaches 1, the exponent c below approaches infinity
	assert(s >= 0.0 && s <= 0.95);

	if (!isEqual(s, 0.0))
	{
		double c = 2.0 / (1.0 - s) - 1.0;
		t = std::pow(t, c);
	}

	return lerp(a, b, t);
}

double variableDeceleration(double a, double b, double t, double s)
{
	// As s approaches 1, the exponent c below approaches infinity
	assert(s >= 0.0 && s <= 0.95);

	if (!isEqual(s, 0.0))
	{
		double c = 2.0 / (1.0 - s) - 1.0;
		t = 1.0 - std::pow(1.0 - t, c);
	}

	return lerp(a, b, t);
}

/*	Sinusoidal easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inSineInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Numerical precision guard
	if (t == 1.0)
		return b;
	else
	{
		double x = 1.0 - std::cos((t * M_PI) / 2.0);
		return lerp(a, b, x);
	}
}

/*	Quadratic easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inQuadInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t * t;
	return lerp(a, b, x);
}

/*	Cubic easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inCubicInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t * t * t;
	return lerp(a, b, x);
}

/*	Quartic easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inQuartInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t * t * t * t;
	return lerp(a, b, x);
}

/*	Quintic easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inQuintInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t * t * t * t * t;
	return lerp(a, b, x);
}

/*	Exponential easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inExpoInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Ignore asymptote at 0
	if (t == 0.0)
		return a;
	else
	{
		double x = std::pow(2, 10.0 * t - 10.0);
		return lerp(a, b, x);
	}
}

/*	Circular easing in over range [a, b], accelerating from zero velocity
	Parameterized by t, with domain [0, 1]    */
double inCircInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = 1.0 - std::sqrt(1.0 - t * t);
	return lerp(a, b, x);
}

/*	Sinusoidal easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outSineInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Numerical precision guard
	if (t == 1.0)
		return b;
	else
	{
		double x = std::sin((t * M_PI) / 2.0);
		return lerp(a, b, x);
	}
}

/*	Quadratic easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outQuadInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = 1.0 - std::pow((1.0 - t), 2);
	return lerp(a, b, x);
}

/*	Cubic easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outCubicInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = 1.0 - std::pow((1.0 - t), 3);
	return lerp(a, b, x);
}

/*	Quartic easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outQuartInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = 1.0 - std::pow((1.0 - t), 4);
	return lerp(a, b, x);
}

/*	Quintic easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outQuintInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = 1.0 - std::pow((1.0 - t), 5);
	return lerp(a, b, x);
}

/*	Exponential easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outExpoInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Ignore asymptote at 1
	if (t == 1.0)
		return b;
	else
	{
		double x = 1 - std::pow(2, -10.0 * t);
		return lerp(a, b, x);
	}
}

/*	Circular easing out over range [a, b], decelerating to zero velocity
	Parameterized by t, with domain [0, 1]    */
double outCircInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = std::sqrt(1.0 - std::pow(t - 1.0, 2));
	return lerp(a, b, x);
}

/*	Sinusoidal easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutSineInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Numerical precision guard
	if (t == 1.0)
		return b;
	else
	{
		double x = (1.0 - std::cos(M_PI * t)) / 2.0;
		return lerp(a, b, x);
	}
}

/*	Quadratic easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutQuadInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t < 0.5 ? 2.0 * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 2) / 2.0;
	return lerp(a, b, x);
}

/*	Cubic easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutCubicInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t < 0.5 ? 4.0 * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 3) / 2.0;
	return lerp(a, b, x);
}

/*	Quartic easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutQuartInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t < 0.5 ? 8.0 * t * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 4) / 2.0;
	return lerp(a, b, x);
}

/*	Quintic easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutQuintInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t < 0.5 ? 16.0 * t * t * t * t * t : 1.0 - std::pow(-2.0 * t + 2.0, 5) / 2.0;
	return lerp(a, b, x);
}

/*	Exponential easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutExpoInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	// Ignore asymptotes at 0 and 1
	if (t == 0.0)
		return a;
	else if (t == 1.0)
		return b;
	else
	{
		double x = t < 0.5 ? std::pow(2.0, 20.0 * t - 10.0) / 2.0 : (2.0 - std::pow(2.0, -20.0 * t + 10.0)) / 2.0;
		return lerp(a, b, x);
	}
}

/*	Circular easing in and out over range [a, b], accelerating until halfway, then decelerating
	Parameterized by t, with domain [0, 1]    */
double inOutCircInterp(double a = 0.0, double b = 1.0, double t = 0.5)
{
	assert(t >= 0.0 && t <= 1.0);

	double x = t < 0.5 ? (1.0 - std::sqrt(1.0 - std::pow(2.0 * t, 2))) / 2.0 : (std::sqrt(1.0 - std::pow(-2.0 * t + 2.0, 2)) + 1.0) / 2.0;
	return lerp(a, b, x);
}

// ------ Misc ------

/*	Provides a relative comparison of two doubles using the machine epsilon
	The machine epsilon will be scaled to account for very large inputs
	The machine epsilon will not be scaled below its default as this is the smallest representable value    */
bool isEqual(double a, double b)
{
	return std::abs(a - b) < std::numeric_limits<double>::epsilon() * std::max( 1.0, std::max(std::abs(a), std::abs(b)) );
}

/*	Provides a relative comparison of two doubles using a custom epsilon
	Useful when comparing the results of approximation functions which have a certain margin of error    */
bool isEqualWithin(double a, double b, double epsilon)
{
	return std::abs(a - b) < epsilon;
}

double remap(double v, double low1, double high1, double low2, double high2)
{
	double divisor = high1 - low1;
	return MRS::isEqual(divisor, 0.0) ? 0.0 : low2 + (v - low1) * (high2 - low2) / (high1 - low1);
}

int remap(int v, int low1, int high1, int low2, int high2)
{
	int divisor = high1 - low1;
	return divisor == 0 ? 0 : (int)std::round(low2 + (v - low1) * (high2 - low2) / (double)(high1 - low1));
}

int pow(int x, unsigned int exp)
{
	if (exp == 0) return 1;
	if (exp == 1) return x;

	int tmp = pow(x, exp / 2);
	if (exp % 2 == 0)
		return tmp * tmp;
	else
		return x * tmp * tmp;
}

/*	For a positive integer n, n! provides the product of all positive integers less than or equal to n
	The empty product is an exception, whereby, 0! = 1
	There are n! ways to arrange n objects in a sequence

	n = size of set    */
unsigned int factorial(unsigned int n)
{
	unsigned int result = 1;
	for (unsigned int i = 2; i <= n; ++i)
		result *= i;

	return result;
}

/*	There are ( n!/(n-k)! ) ordered, k-sized subset permutations of a n-sized set (each combination has k! permutations, ie. selection order matters)

	n = size of set
	k = size of subset, k <= n    */
unsigned int permutations(unsigned int n, unsigned int k)
{
	assert(k <= n);

	// If (n - k) is large, the general formula is not very efficient so we optimize
	unsigned int result = 1;
	for (unsigned int i = n - k + 1; i <= n; ++i)
		result *= i;

	return result;
}

/*	There are ( (n!/(n-k)!)/k! ) unordered, k-sized subset combinations in a n-sized set, since each k-sized combination can be selected in k! different ways
	- This is known as the binomial coefficient because it represents the coefficient of each term ( x^k ) in the expanded binomial formula ( (1+x)^n )
	- For each term, we are asking, how many ways are there to choose k-number of x's from a fixed set containing n-number of (1, x) elements
	- Eg. (1+x)^2 = 1 + 2x + x^2	: there are two ways to choose one x (middle term) from the set { (1,x), (1,x) }, either (1,x) or (x,1)

	n = size of set
	k = size of subset, k <= n    */
unsigned int binomialCoefficient(unsigned int n, unsigned int k)
{
	assert(k <= n);

	return permutations(n, k) / factorial(k);
}

/*	For some set of values {a-0, a-1 ... a-(n-1), a-n} :
	- We can find the finite forward difference by repeatedly calculating the difference between each sequential element (reverse order)
	- Eg. For a set with three elements, the following equations would be used : (a-2 - a-1) - (a-1 - a0)
	- This is called a second order difference equation as there are 2 equations
	- There is a formula which allows us to reduce the set of equations into a single sum of all elements, each multiplied by a respective coefficient
	
	This function returns the coefficient for the element at index n-k
	n = order (size of set - 1)
	k = current iteration, k <= n    */
int forwardDifferenceCoefficient(unsigned int n, unsigned int k)
{
	return pow(-1, k) * binomialCoefficient(n, k);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------