#include "spline_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Resources
	---------
	Arc-length parameterization
	http://algorithmist.net/docs/arcparam.pdf
	https://gamedev.stackexchange.com/questions/5373/moving-ships-between-two-planets-along-a-bezier-missing-some-equations-for-acce/5427#5427
	RMF (normal plane projection)
	https://onlinelibrary.wiley.com/doi/abs/10.1111/1467-8659.1130449
	RMF (double reflection quaternion)
	https://www.researchgate.net/publication/261666607_Computing_Rotation_Minimizing_Frames_using_Quaternions
	https://eater.net/quaternions
	RMF (double reflection vector)
	https://www.researchgate.net/publication/220183621_Computation_of_rotation_minimizing_frames    */

	Spline::Spline() {}
	Spline::~Spline() {}

// ------ Approximate ------

/*	Description
	-----------
	Provides an approximate value for the curvature at the point where the current tangent was sampled
	This function expects non-normalized tangent vectors
	
	Args
	----
	tDelta = The difference in the natural parameter t between the two tangent vectors    */
double Spline::approximateCurvature(const MVector& vTangentPrevious, const MVector& vTangentCurrent, double tDelta)
{
	MVector vUnitTangentPrevious = vTangentPrevious.normal();
	MVector vUnitTangentCurrent = vTangentCurrent.normal();
	MVector vUnitTangentDelta = vUnitTangentCurrent - vUnitTangentPrevious;
	vUnitTangentDelta /= tDelta;

	return vUnitTangentDelta.length() / vTangentCurrent.length();
}

// ------ Parameterization ------

/*	Description
	-----------
	This function will produce an array of values for the natural parameter t by uniformly incrementing over the given domain
	The number of values will be equal to the size of the given array argument
	The default domain of a curve is [0,1], however when a curve is unclamped we may need to limit this domain to a restricted knot interval

	A parametric curve is defined by its natural parameter t
	The time taken to travel between sample points at uniform increments of t is constant
	However, if the distribution of control points is non-uniform then the arc-length of a segment (defined by a uniform increment of t) will vary
	The magnitude of each tangent (ie. the parametric speed) will also vary as it is interpreted as the rate of change in the normalized arc-length with respect to the parameter    */
void Spline::computeNaturalParameters(std::vector<double>& outParameters, double lowerBound, double upperBound)
{
	unsigned int sampleCount = (int)outParameters.size();

	assert(sampleCount >= 2);

	double t = lowerBound;
	double step = (upperBound - lowerBound) / (double)(sampleCount - 1);

	for (unsigned int i = 0; i < sampleCount; i++)
	{
		outParameters[i] = t;
		t += step;
	}

	// Ensure precision error does not interfere with boundary conditions
	outParameters[sampleCount - 1] = upperBound;
}

/*	Description
	-----------
	Produces an array of values for the natural parameter t, each corresponding to a uniform increment of the arc-length
	- The number of values will be equal to the size of the given lengths array
	- The calculation is independent of the curve type as it operates on pre-sampled curve data

	The curve can be parameterized in terms of its arc-length such that it has unit parametric speed (uniform motion)
	Consider s to be the normalized arc-length of the curve for some value of t
	There is an increasing relationship between s and t over the domain [0,1]
	This relationship can be expressed as L(t)=s, L:[0,1]->[0,1]
	Subsequently the relationship can be expressed as L^-1(s)=t, L^-1:[0,1]->[0,1]
	The parameterization of the curve P(t) in terms of its arc-length can be expressed as a composite function Q(s) = P(L^-1(s))
	It is very difficult to find a closed-form solution to L^-1 however it can be approximated numerically

	Args
	----
	lengths = An array of values containing the length of the curve at uniform increments of the its natural parameter
			  The output vector will be of the same size    */
void Spline::computeArcLengthParameters(const std::vector<double>& lengths, std::vector<double>& outParameters)
{
	unsigned int sampleCount = (int)lengths.size();
	assert(sampleCount >= 2);

	outParameters.resize(sampleCount);
	outParameters[0] = 0.0;

	// Using the total arc-length, iterate over uniform increments and approximate values of t
	// We are essentially solving the function L^-1(s) described above, however our values for s are not normalized (the concept remains the same)
	// We don't want to approximate the end parameters of the curve as they should always be 0 and 1
	double percent = 0.0;
	double totalLength = lengths[sampleCount - 1];
	double step = 1.0 / (double)(sampleCount - 1);

	// The number of iterations accounts for the two end parameters
	for (unsigned int i = 1; i < sampleCount - 1; i++)
	{
		percent += step;
		double targetLength = percent * totalLength;

		// Complete a binary search to find the closest tLength which is smaller than the targetLength
		// We then just need to retrieve the t value which was used to calulate the tLength
		unsigned int low = 0;
		unsigned int high = sampleCount;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		// The last iteration of the search may cause the index to increase due to the low value increasing on the second last iteration
		// In this case, the tLength at the current index may be larger than the targetLength
		// We must complete one final check and decrement the index if this was the case
		if (tLength > targetLength)
			index -= 1;

		assert(lengths[index] <= targetLength);
		assert(lengths[index + 1] > targetLength);

		// To retrieve the t value for the tLength we can calculate, t = index / (sampleCount - 1)
		// However, we can increase the accuracy by determining the skew of the target length in relation to the lengths before and after
		double tLengthBefore = lengths[index];
		double tLengthAfter = lengths[index + 1];
		double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((double)sampleCount - 1.0);
		outParameters[i] = t;
	}

	outParameters[sampleCount - 1] = 1.0;
}

/*	Description
	-----------
	Calculates an equivalent natural parameter approximation for the given arc-length parameter
	- This calculation is independent of the curve type as it operates on pre-sampled curve data

	Args
	----
	arcLengthParameter = The arc-length parameter from which to calculate the equivalent natural parameter
	lengths = An array of values containing the length of the curve at uniform increments of the its natural parameter    */
double Spline::arcLengthToNaturalParameter(double arcLengthParameter, const std::vector<double>& lengths)
{
	if (arcLengthParameter == 0.0 || arcLengthParameter == 1.0)
		return arcLengthParameter;

	unsigned int sampleCount = (int)lengths.size();
	assert(sampleCount >= 2);

	double totalLength = lengths[sampleCount - 1];
	double targetLength = arcLengthParameter * totalLength;
	double tLength = 0.0;

	unsigned int low = 0;
	unsigned int high = sampleCount;
	unsigned int index;

	while (low < high)
	{
		index = low + (high - low) / 2;
		tLength = lengths[index];

		if (tLength < targetLength)
			low = index + 1;
		else
			high = index;
	}

	if (tLength > targetLength)
		index -= 1;

	assert(lengths[index] <= targetLength);
	assert(lengths[index + 1] > targetLength);

	double tLengthBefore = lengths[index];
	double tLengthAfter = lengths[index + 1];
	double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((double)sampleCount - 1.0);
	return t;
}

/*	Description
	-----------
	Calculates an equivalent normalized arc-length approximation for the given natural parameter
	- This calculation is independent of the curve type as it operates on pre-sampled curve data
	
	Args
	----
	naturalParameter = The natural parameter from which to calculate the equivalent arc-length parameter
	lengths = An array of values containing the length of the curve at uniform increments of the its natural parameter    */
double Spline::naturalToArcLengthParameter(double naturalParameter, const std::vector<double>& lengths)
{
	if (naturalParameter == 0.0 || naturalParameter == 1.0)
		return naturalParameter;

	unsigned int sampleCount = (int)lengths.size();
	assert(sampleCount >= 2);

	double indexCurrent = naturalParameter * (sampleCount - 1);
	unsigned int indexBefore = (unsigned)indexCurrent;

	double arcLengthBefore = lengths[indexBefore];
	double arcLengthAfter = lengths[indexBefore + 1];

	// Determine the skew of the current position towards the arc-lengths before and after
	double arcLength = arcLengthBefore + (arcLengthAfter - arcLengthBefore) * (indexCurrent - indexBefore);

	// Normalize
	double totalLength = lengths[sampleCount - 1];
	double arcLengthParam = arcLength / totalLength;
	return arcLengthParam;
}

// ------ RMF ------

/*	Description
	-----------
	Computes an approximate normal vector based on the RMF normal plane projection method
	This approach is much less accurate than the double reflection method but is much simpler to implement

	This function expects unit vectors    */
MVector Spline::computeProjectedNormalRMF(const MVector& vNormalPrevious, const MVector& vTangentCurrent)
{
	// Get the component of the previous normal which is parallel to the current tangent
	double magNormalPreviousInTangent = vNormalPrevious * vTangentCurrent;
	MVector vNormalPreviousInTangent = magNormalPreviousInTangent * vTangentCurrent;
	// Subtracting from the previous normal gives us a vector which is perpendicular to the current tangent
	// This represents the projection of the previous normal onto the plane which is perpendicular to the current tangent
	MVector vNormalCurrent = vNormalPrevious - vNormalPreviousInTangent;
	vNormalCurrent.normalize();

	return vNormalCurrent;
}

/*	Description
	-----------
	Computes a quaternion which can be used to reflect the previous normal accross two planes, resulting in the current normal (this method is very accurate)
	Because quaternion rotations can be composed, we do not need to calculate the normal vector for each sample (just for those which which we want to output)
	We therefore return a quaternion representing the double reflection instead of calculating a normal vector

	This function expects unit vectors

	Algorithm
	---------
	An initial frame U-0 is formed using the principal normal vector
	For each sample point p-i, from p-1 to p-n :
	1.	Calculate the vector v-i from p-(i-1) to p-i and normalize it
	2.	Form a pure quaternion using v-i such that qr1 = [0, ||v-i||], this is our first reflection quaternion
	3.	Form a pure quaternion using the normalized tangent vector t-(i-1) such that qt = [0, ||t-(i-1)||]
	4.	Multiply qr1 * qt
		- The result can be interpreted as the vector t-(i-1) projected onto a plane which is normal to and bisecting the vector v-i
		- The projected vector is also rotated by 90 degrees around v-i as a pure quaternion has w=0 (ie. cos(pi/2) = 0)
	5.	Multiply the result by qr1, such that qt' = qr1 * qt * qr1
		- For a standard quaternion rotation, we would have instead multiplied the result by the inverse of qr1
		- This would have effectively undone the projection (normalized the quaternion) and rotated the vector t-(i-1) another 90 degrees around v-i
		- Instead, multiplying by qr1 again, reflects the vector t-(i-1) in the plane
		- The quaternion is normalized in the opposite direction and the initial rotation is negated
	6.	Let t' be the vector component of the resulting quaternion qt', representing the reflection of t-(i-1) in the first reflection plane
	7.	Calculate the vector from t' to t-i and normalize it
	8.	Form a pure quaternion using the above vector such that qr2 = [0, ||t-i - t'||], this is our second reflection quaternion

	We now need to compute the double reflection of the normal vector n-(i-1) in these planes (ie. qr1 and qr2), resulting in the normal vector n-i

	Unoptimised computation of n-i :
	9.	Form a pure quaternion using the normalized normal vector n-(i-1) such that qn = [0, ||n-(i-1)||]
	10.	Calculate the reflection of qn in the plane qr1 such that qn' = qr1 * qn * qr1
	11.	Calculate the reflection of qn' in the plane qr2 such that qn'' = qr2 * qn' * qr2

	Optimised computation of n-i
	12.	Using the above equations, substitute qn' into qn''
		- qn'' = qr2 * (qr1 * qn * qr1) * qr2
		- qn'' = (qr2 * qr1) * qn * (qr1 * qr2)
		- qn'' = (qr2 * qr1) * qn * (qr2 * qr1)*
		- The final equation represents a special property of quaternions which allows us to combine two reflections into a single rotation
		- Therefore we only have to calulate qr2 * qr1 once, then calculate its conjugate which is a much cheaper operation
	13. Finally, let n-i be the vector component of qn''    */
MQuaternion Spline::computeDoubleReflectionRMF(const MVector& vPointPrevious, const MVector& vPointCurrent, const MVector& vTangentPrevious, const MVector& vTangentCurrent)
{
	MQuaternion qTangentPrevious{ vTangentPrevious.x, vTangentPrevious.y, vTangentPrevious.z, 0.0 };
	MQuaternion qTangentCurrent{ vTangentCurrent.x, vTangentCurrent.y, vTangentCurrent.z, 0.0 };

	// qr1
	MVector vPointDisplacement = vPointCurrent - vPointPrevious;
	vPointDisplacement.normalize();
	MQuaternion qReflectionPlane1{ vPointDisplacement.x, vPointDisplacement.y, vPointDisplacement.z, 0.0 };

	// qr2
	// The result of multiplying two pure quaternions is not necessarily a pure quaternion
	MQuaternion qTangentPreviousProjected = quaternionPureMultiply(qReflectionPlane1, qTangentPrevious);
	// However the reflected vector will be pure as we know it is just the reflection (it will also be normalized)
	MQuaternion qTangentPreviousReflected = quaternionMultiply(qTangentPreviousProjected, qReflectionPlane1);
	MQuaternion qReflectionPlane2 = qTangentCurrent - qTangentPreviousReflected;
	qReflectionPlane2.normalizeIt();

	// Double reflection
	MQuaternion qReflectionPlaneComposition = quaternionPureMultiply(qReflectionPlane2, qReflectionPlane1);
	
	return qReflectionPlaneComposition;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Resources
	---------
	Bezier equations
	https://en.wikipedia.org/wiki/Bézier_curve
	https://www.uio.no/studier/emner/matnat/ifi/nedlagte-emner/INF3320/h03/undervisningsmateriale/lecture8.pdf <-- See section on derivatives    */

Bezier::Bezier() {};
Bezier::~Bezier() {};

// ------ Parameterization ------

/*	Description
	-----------
	Produces an array of lengths (of the current size), each corresponding to a uniform increment of the natural parameter
	- These values are used as the basis for approximating certain parameterization functions
	- Used when a mapping between arc-length values and natural parameters is required    */
void Bezier::computeLengths(const std::vector<MVector>& controlPoints, std::vector<double>& outlengths, std::vector<MVector>* outPoints)
{
	unsigned int sampleCount = (int)outlengths.size();
	assert(sampleCount >= 2);

	if (outPoints)
		outPoints->resize(sampleCount);

	// Cache arc-lengths by uniformly incrementing over the curves natural parameter t
	double step = 1.0 / (double)(sampleCount - 1);
	double t = 0.0;
	double tLength = 0.0;
	outlengths[0] = 0.0;
	MVector previousPoint = sampleCurve(0.0, controlPoints);
	if (outPoints)
		(*outPoints)[0] = previousPoint;

	for (unsigned int i = 1; i < sampleCount; i++)
	{
		t += step;

		MVector currentPoint = sampleCurve(t, controlPoints);
		MVector diff = previousPoint - currentPoint;
		if (outPoints)
			(*outPoints)[i] = currentPoint;

		tLength += diff.length();
		outlengths[i] = tLength;
		previousPoint = currentPoint;
	}
}

// ------ Sample ------

/*	Description
	-----------
	A Bernstein polynomial of degree p is described by a linear combination of p+1 Bernstein basis polynomials, each also of degree p
	The basis share the same indeterminate x, with the same constant p and a unique constant k
	This function will map the result of a specific Bernstein basis (defined by its constants p and k) to a specific value of the variable x
	The summation of results for some arbitrary value x, for each Bernstein basis with degree p, is equal to 1 (the basis are said to form a 'partition of unity')

	Args
	----
	x = Indeterminate
	degree = Degree of polynomial
	k = Current basis iteration    */
double Bezier::sampleBasis(double x, unsigned int degree, unsigned int k)
{
	return binomialCoefficient(degree, k) * std::pow(x, k) * std::pow((1 - x), degree - k);
}

/*	Description
	-----------
	Calculates a point at the given natural parameter t for a non-rational Bezier defined by the given control points
	A linear combination of Bernstein basis polynomials of degree p, each multiplied by a respective Bernstein coefficient, forms a Bernstein polynomial of degree p
	In our case, each Bernstein coefficient represents a point in R3 (p+1 points describe a p-degree Bezier curve)
	
	Note, this function exists to avoid confusion with the sampleDerivative function which is actually capable of sampling points (order = 0) and derivatives (order > 0)
	
	Args
	----
	t = Natural parameter of curve    */
MVector Bezier::sampleCurve(double t, const std::vector<MVector>& controlPoints)
{
	MVector vCurveSample;
	unsigned int degree = (int)controlPoints.size() - 1;

	for (unsigned int k = 0; k <= degree; k++)
	{
		double bernsteinBasisSample = sampleBasis(t, degree, k);
		vCurveSample += controlPoints[k] * bernsteinBasisSample;
	}

	return vCurveSample;
}

/*	Description
	-----------
	Calculates either a position or a derivative of the given order for a non-rational Bezier at the given natural parameter t
	The resulting vector will not be normalized
	
	Args
	----
	order = If order == 0, the output represents a position sampled along the curve
			If order > 0, the output represents a vector for the r-th derivative sampled along the curve
	t = Natural parameter of curve    */
MVector Bezier::sampleDerivative(unsigned int order, double t, const std::vector<MVector>& controlPoints)
{
	assert(controlPoints.size() - 1 - order >= 0);

	MVector vDerivativeSample;
	unsigned int degree = (int)controlPoints.size() - 1;
	unsigned int summationCoefficient = permutations(degree, order);

	for (unsigned int k = 0; k <= degree - order; k++)
	{
		MVector vForwardDifference;

		for (unsigned int i = 0; i <= order; i++)
		{
			// For each iteration of k, the coefficients remain the same as we are iterating over the same i values, however a single point will change from the subset of points being iterated
			vForwardDifference += forwardDifferenceCoefficient(order, order - i) * controlPoints[k + i];
		}

		double bernsteinBasisSample = sampleBasis(t, degree - order, k);
		vDerivativeSample += vForwardDifference * bernsteinBasisSample;
	}

	vDerivativeSample *= summationCoefficient;
	return vDerivativeSample;
}

/*	Description
	-----------
	Provides an accurate value for the curvature at a given value of the natural parameter t

	The calculation for curvature is synonymous with the rate at which the tangent changes in relation to the arc-length of the curve
	The calculation uses the gemometric properties of a curve and is therefore relative to the size of that curve
	In general the value of an arbitrary sample should be normalized in relation to a sequence of other samples

	Args
	----
	t = Natural parameter of the curve */
double Bezier::sampleCurvature(double t, const std::vector<MVector>& controlPoints)
{
	MVector vFirstDerivative = sampleDerivative(1, t, controlPoints);
	MVector vSecondDerivative = sampleDerivative(2, t, controlPoints);
	double firstLength = vFirstDerivative.length();

	return (vFirstDerivative ^ vSecondDerivative).length() / (firstLength * firstLength * firstLength);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Resources
	---------
	Trigonometric Bezier equations
	https://www.sciencedirect.com/science/article/pii/S0893965908001262
	Closest point on curve
	https://hal.inria.fr/file/index/docid/518379/filename/Xiao-DiaoChen2007c.pdf
*/

CubicTBezier::CubicTBezier() {}
CubicTBezier::~CubicTBezier() {}

// ------ Parameterization ------

/*	Description
	-----------
	Produces an array of lengths (of the current size), each corresponding to a uniform increment of the natural parameter
	- These values are used as the basis for approximating certain parameterization functions
	- Used when a mapping between arc-length values and natural parameters is required
	
	Args
	----
	shape1 = The first parameter used to adjust the shape of the curve, range = [-1, 1]
	shape2 = The second parameter used to adjust the shape of the curve, range = [-1, 1]    */
void CubicTBezier::computeLengths(double shape1, double shape2, const std::vector<MVector>& controlPoints, 
	std::vector<double>& outLengths, std::vector<MVector>* outPoints)
{
	unsigned int sampleCount = (int)outLengths.size();
	assert(sampleCount >= 2);
	assert(controlPoints.size() == 4);
	assert(shape1 >= -1.0 && shape1 <= 1.0);
	assert(shape2 >= -1.0 && shape2 <= 1.0);

	if (outPoints)
		outPoints->resize(sampleCount);

	// Cache arc-lengths by uniformly incrementing over the curves natural parameter t
	double step = 1.0 / (double)(sampleCount - 1);
	double t = 0.0;
	double tLength = 0.0;
	outLengths[0] = 0.0;
	MVector previousPoint = sampleCurve(0.0, shape1, shape2, controlPoints);
	if (outPoints)
		(*outPoints)[0] = previousPoint;

	for (unsigned int i = 1; i < sampleCount; i++)
	{
		t += step;

		MVector currentPoint = sampleCurve(t, shape1, shape2, controlPoints);
		MVector diff = previousPoint - currentPoint;
		if (outPoints)
			(*outPoints)[i] = currentPoint;

		tLength += diff.length();
		outLengths[i] = tLength;
		previousPoint = currentPoint;
	}
}

/*	Description
	-----------
	This parameterization function is specific to this curve type and is designed to stabalize the mid point of the curve between control points P1 and P2
	It provides the ability to stretch the chain (eg. arm) whilst maintaining the integrity of its initial form
	The number of output parameters will be equal to the size of the given lengths array argument

	The calculation splits the curve into two segments determined by the point on the curve closest to the point half way between control points P1 and P2
	- We will call the natural parameter of the curve at this point tStable
	We want to parameterize each segment of the curve in terms of its arc-length, such that each segment has unit parametric speed (uniform motion)
	- There will be an instantaneous change in velocity at tStable where the two segments meet
	- Each segment should be responsible for calculating half of the output parameters    */
void CubicTBezier::computeSplitLengthParameters(double shape1, double shape2, const std::vector<MVector>& controlPoints, 
	const std::vector<MVector>& points, const std::vector<double>& lengths, std::vector<double>& outParameters)
{
	unsigned int sampleCount = (int)lengths.size();
	assert(sampleCount >= 2);
	assert(sampleCount == points.size() == tangents.size());

	outParameters.resize(sampleCount);
	outParameters[0] = 0.0;

	if (sampleCount == 2)
	{
		outParameters[1] = 1.0;
		return;
	}

	// We can approximate the stable parameter by minimizing the distance to our target point
	// We could also minimize the dot product of the displacement vector with the tangent vector: (p - q(t)) . q'(t) = 0
	// However the latter solution becomes unstable when a small sample count is used and is more compuationally heavy (requires derivative at each point)
	// We know the closest point will occur at close to 50% of the curves natural parameter so we will begin our search from here
	// This also avoids the possibility of finding a local minimum instead of the global minimum for each point which is what we are looking for

	// --- Closest Point Joint0 ---
	MPoint targetPoint = controlPoints[1] + (controlPoints[2] - controlPoints[1]) / 2;
	// This will always be larger than 0 and smaller than sampleCount-1
	unsigned int closestIndex = sampleCount / 2;
	MVector vDisplacement = targetPoint - points[closestIndex];
	double closestMag = vDisplacement.length();
	vDisplacement = targetPoint - points[closestIndex - 1];
	double earlyMag = vDisplacement.length();
	vDisplacement = targetPoint - points[closestIndex + 1];
	double lateMag = vDisplacement.length();

	char direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex == 0 && direction == -1)
				break;
			if (closestIndex == sampleCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - points[closestIndex + direction];
			nextMag = vDisplacement.length();
		}
	}

	// We can calculate, tStable = closestIndex / (sampleCount - 1)
	// However, we can increase the accuracy by determining the skew of the current dot product in relation to the dot product before or after
	// We are determining if the minimum occurs before or after the current index by querying whether the sign of the dot product calculations changes
	// If the sign does not change then the minimum is not skewed, indicating the closest index is one of the end points
	double tStable = closestIndex / (double)(sampleCount - 1);

	double earlyProduct;
	double lateProduct;
	vDisplacement = targetPoint - points[closestIndex]; vDisplacement.normalize();
	MVector vTangent = sampleFirstDerivative(closestIndex / (double)(sampleCount - 1), shape1, shape2, controlPoints);
	double closestProduct = vDisplacement * vTangent;

	if (closestIndex == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - points[closestIndex - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex - 1)/ (double)(sampleCount - 1), shape1, shape2, controlPoints);
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex == sampleCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - points[closestIndex + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex + 1) / (double)(sampleCount - 1), shape1, shape2, controlPoints);
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);

	if (isMinimumEarly)
	{
		// The minimum occurs before tStable
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = 1.0 / (double)(sampleCount - 1);
		tStable = tStable - step * minimumRatio;
	}
	else if (isMinimumLate)
	{
		// The minimum occurs after tStable
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = 1.0 / (double)(sampleCount - 1);
		tStable = tStable + step * minimumRatio;
	}

	// --- Sample Distribution ---
	// When parameterizing the curve, we attempt to evenly distribute samples between each segment
	// In the case where the sampleCount is not evenly divisible by the 2 segments, the first segments will have preference and receive the extra samples
	// There are three constant points (start, tStable, end), from this we can determine the distribution:
	unsigned int remainingCount = sampleCount - 3;
	unsigned int segment0SampleCount = (unsigned)std::ceil(remainingCount / 2.0);
	unsigned int segment1SampleCount = remainingCount - segment0SampleCount;
	unsigned int stableIndex = segment0SampleCount + 1; // +1 for start point

	// --- Segment0 ---
	// Using the total arc-length of segment0, iterate over uniform increments and approximate values of t for (0.0, tStable)
	double segment0ArcLength = naturalToArcLengthParameter(tStable, lengths) * lengths[sampleCount - 1];
	double segment0Step = 1.0 / (segment0SampleCount + 1);
	double segment0Percent = 0.0;

	for (unsigned int i = 1; i < stableIndex; ++i)
	{
		segment0Percent += segment0Step;
		double targetLength = segment0Percent * segment0ArcLength;

		// Complete a binary search to find the closest tLength which is smaller than the targetLength
		// We then just need to retrieve the t value which was used to calulate the tLength
		unsigned int low = 0;
		unsigned int high = isMinimumLate ? closestIndex + 1 : closestIndex;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		// The last iteration of the search may cause the index to increase due to the low value increasing on the second last iteration
		// In this case, the tLength at the current index may be larger than the targetLength
		// We must complete one final check and decrement the index if this was the case
		if (tLength > targetLength)
			index -= 1;

		assert(lengths[index] <= targetLength);
		assert(lengths[index + 1] > targetLength);

		// To retrieve the t value for the tLength we can calculate, t = index / (sampleCount - 1)
		// However, we can increase the accuracy by determining the skew of the target length in relation to the lengths before and after
		double tLengthBefore = lengths[index];
		double tLengthAfter = lengths[index + 1];
		double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / (double)(sampleCount - 1);
		outParameters[i] = t;
	}

	outParameters[sampleCount / 2] = tStable;

	// --- Segment1 ---
	// Using the total arc-length of segment1, iterate over uniform increments and approximate values of t for (tStable, 1.0)
	double segment1ArcLength = lengths[sampleCount - 1] - segment0ArcLength;
	double segment1Step = 1.0 / (segment1SampleCount + 1);
	double segment1Percent = 0.0;

	for (unsigned int i = stableIndex + 1; i < sampleCount - 1; ++i)
	{
		segment1Percent += segment1Step;
		double targetLength = segment0ArcLength + segment1Percent * segment1ArcLength;

		unsigned int low = isMinimumEarly ? closestIndex - 1 : closestIndex;
		unsigned int high = sampleCount;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		if (tLength > targetLength)
			index -= 1;

		assert(lengths[index] <= targetLength);
		assert(lengths[index + 1] > targetLength);

		double tLengthBefore = lengths[index];
		double tLengthAfter = lengths[index + 1];
		double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / (double)(sampleCount - 1);
		outParameters[i] = t;
	}

	outParameters[sampleCount - 1] = 1.0;
}

/*	Description
	-----------
	Calculates an equivalent natural parameter approximation for the given split-length parameter
	We will calculate the stable parameter from which we can determine the arc-lengths of each segment
	We can then determine the arc-length at the given parameter by determining its proportional position within the segment's parameterized range
	From the arc-length we can determine the natural parameter

	Args
	----
	splitLengthParameter = The split-length parameter from which to calculate the equivalent natural parameter, range = [0.0, 1.0]
	controlPoints = The four positions used to define the shape of the curve
	points = An array of positions sampled along the curve at uniform increments of its natural parameter
	tangents = An array of tangents sampled along the curve at uniform increments of its natural parameter
	lengths = An array of values containing the length of the curve at uniform increments of its natural parameter    */
double CubicTBezier::splitLengthToNaturalParameter(double splitLengthParameter, double shape1, double shape2, 
	const std::vector<MVector>& controlPoints, const std::vector<MVector>& points, const std::vector<double>& lengths)
{
	if (splitLengthParameter == 0.0 || splitLengthParameter == 1.0)
		return splitLengthParameter;

	unsigned int sampleCount = (int)lengths.size();
	assert(sampleCount >= 2);
	assert(sampleCount == points.size() == tangents.size());
	
	// --- Closest Point Joint0 ---
	MPoint targetPoint = controlPoints[1] + (controlPoints[2] - controlPoints[1]) / 2;
	// This will always be larger than 0 but may be equal to sampleCount-1
	unsigned int closestIndex = sampleCount / 2;
	MVector vDisplacement = targetPoint - points[closestIndex];
	double closestMag = vDisplacement.length();
	vDisplacement = targetPoint - points[closestIndex - 1];
	double earlyMag = vDisplacement.length();
	
	double lateMag;
	if (closestIndex == sampleCount - 1)
		lateMag = closestMag;
	else
	{
		vDisplacement = targetPoint - points[closestIndex + 1];
		double lateMag = vDisplacement.length();
	}

	char direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex == 0 && direction == -1)
				break;
			if (closestIndex == sampleCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - points[closestIndex + direction];
			nextMag = vDisplacement.length();
		}
	}

	// Calculate the skew
	double tStable = closestIndex / (double)(sampleCount - 1);

	double earlyProduct;
	double lateProduct;
	vDisplacement = targetPoint - points[closestIndex]; vDisplacement.normalize();
	MVector vTangent = sampleFirstDerivative(closestIndex / (double)(sampleCount - 1), shape1, shape2, controlPoints);
	double closestProduct = vDisplacement * vTangent;

	if (closestIndex == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - points[closestIndex - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex - 1) / (double)(sampleCount - 1), shape1, shape2, controlPoints);
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex == sampleCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - points[closestIndex + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex + 1) / (double)(sampleCount - 1), shape1, shape2, controlPoints);
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);

	if (isMinimumEarly)
	{
		// The minimum occurs before tStable
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = 1.0 / (double)(sampleCount - 1);
		tStable = tStable - step * minimumRatio;
	}
	else if (isMinimumLate)
	{
		// The minimum occurs after tStable
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = 1.0 / (double)(sampleCount - 1);
		tStable = tStable + step * minimumRatio;
	}

	// --- Parameterization ---
	double segment0ArcLength = naturalToArcLengthParameter(tStable, lengths) * lengths[sampleCount - 1];
	double segment1ArcLength = lengths[sampleCount - 1] - segment0ArcLength;
	double segmentParamSize = 0.5;

	double targetLength;
	if (splitLengthParameter >= 0.0 && splitLengthParameter < segmentParamSize)
	{
		double segmentRatio = splitLengthParameter / segmentParamSize;
		targetLength = segment0ArcLength * segmentRatio;
	}
	else
	{
		double segmentRatio = (splitLengthParameter - segmentParamSize) / segmentParamSize;
		targetLength = segment0ArcLength + segment1ArcLength * segmentRatio;
	}

	double tLength = 0.0;

	unsigned int low = 0;
	unsigned int high = sampleCount;
	unsigned int index;

	while (low < high)
	{
		index = low + (high - low) / 2;
		tLength = lengths[index];

		if (tLength < targetLength)
			low = index + 1;
		else
			high = index;
	}

	if (tLength > targetLength)
		index -= 1;

	assert(lengths[index] <= targetLength);
	assert(lengths[index + 1] > targetLength);

	double tLengthBefore = lengths[index];
	double tLengthAfter = lengths[index + 1];
	double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / (double)(sampleCount - 1);
	return t;
}

// ------ Sample ------

/*	Description
	-----------
	This function calculates a position at the given parameter of the T-Bezier
	The shape of the curve is defined by the given control points and the two shaping parameters
	The shaping parameters both have ranges of [-1, 1], whereby mid values of 0 describe a standard form cubic Bezier
	- This deviates from the resource listed above where both shaping parameters have a range of [-2, 1]
	- This function will remap the parameters from the expected range of [-1, 1] to the range required by the T-Bezier equations of [-2, 1]
	
	Args
	----
	t = Natural parameter of the Bezier curve, range = [0, 1]
	shape1 = The first parameter used to adjust the shape of the curve, range = [-1, 1]
	shape2 = The second parameter used to adjust the shape of the curve, range = [-1, 1]
	controlPoints = Control points used to define the curve, must contain exactly four points    */
MVector CubicTBezier::sampleCurve(double t, double shape1, double shape2, const std::vector<MVector>& controlPoints)
{
	assert(controlPoints.size() == 4);
	assert(t >= 0.0 && t <= 1.0);
	assert(shape1 >= -1.0 && shape1 <= 1.0);
	assert(shape2 >= -1.0 && shape2 <= 1.0);

	// Remap = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
	double remappedShape1 = -2.0 + (shape1 + 1.0) * 3.0 / 2.0;
	double remappedShape2 = -2.0 + (shape2 + 1.0) * 3.0 / 2.0;
	double sinComp = sin(M_PI_2 * t);
	double cosComp = cos(M_PI_2 * t);
	double subSinComp = 1.0 - sinComp;
	double subCosComp = 1.0 - cosComp;

	double basis0 = std::pow(subSinComp, 2.0) * (1 - shape1 * sinComp);
	double basis1 = sinComp * subSinComp * (2.0 + shape1 - shape1 * sinComp);
	double basis2 = cosComp * subCosComp * (2.0 + shape2 - shape2 * cosComp);
	double basis3 = std::pow(subCosComp, 2.0) * (1.0 - shape2 * cosComp);

	return (basis0 * controlPoints[0]) + (basis1 * controlPoints[1]) + (basis2 * controlPoints[2]) + (basis3 * controlPoints[3]);
}

/*	Description
	-----------
	This function calculates the first derivative at the given parameter of the T-Bezier
	The derivative calculation requires the same data as the above position sampling function

	Args
	----
	t = Natural parameter of the Bezier curve, range = [0, 1]
	shape1 = The first parameter used to adjust the shape of the curve, range = [-1, 1]
	shape2 = The second parameter used to adjust the shape of the curve, range = [-1, 1]
	controlPoints = Control points used to define the curve, must contain exactly four points    */
MVector CubicTBezier::sampleFirstDerivative(double t, double shape1, double shape2, const std::vector<MVector>& controlPoints)
{
	assert(controlPoints.size() == 4);
	assert(t >= 0.0 && t <= 1.0);
	assert(shape1 >= -1.0 && shape1 <= 1.0);
	assert(shape2 >= -1.0 && shape2 <= 1.0);

	// Remap = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
	double remappedShape1 = -2.0 + (shape1 + 1.0) * 3.0 / 2.0;
	double remappedShape2 = -2.0 + (shape2 + 1.0) * 3.0 / 2.0;
	double sinComp = sin(M_PI_2 * t);
	double cosComp = cos(M_PI_2 * t);
	double subSinComp = 1.0 - sinComp;
	double subCosComp = 1.0 - cosComp;
	double multSinComp = shape1 * sinComp;
	double multCosComp = shape2 * cosComp;

	// Each of the following derivatives has been calculated using the product and/or chain rules:
	// B'(t) = a'bc + ab'c + abc'
	// B'(t) = a'(b) * b'
	double basis0 = subSinComp * -2.0 * cosComp * (1.0 - multSinComp) + std::pow(subSinComp, 2.0) * (-shape1 * cosComp);
	double basis1 = cosComp * subSinComp * (2.0 + shape1 - multSinComp) + sinComp * -cosComp * (2.0 + shape1 - multSinComp) + sinComp * subSinComp * -shape1 * cosComp;
	double basis2 = -sinComp * subCosComp * (2.0 + shape2 - multCosComp) + cosComp * sinComp * (2.0 + shape2 - multCosComp) + cosComp * subCosComp * shape2 * sinComp;
	double basis3 = subCosComp * 2.0 * sinComp * (1.0 - multCosComp) + std::pow(subCosComp, 2.0) * (shape2 * sinComp);

	return (basis0 * controlPoints[0]) + (basis1 * controlPoints[1]) + (basis2 * controlPoints[2]) + (basis3 * controlPoints[3]);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Resources
	---------
	B-spline algorithms
	https://en.wikipedia.org/wiki/B-spline
	https://en.wikipedia.org/wiki/De_Boor%27s_algorithm
	https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/B-spline/bspline-curve-prop.html
	https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/B-spline/bspline-basis.html
	https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/de-Boor.html
	B-spline derivatives
	https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/B-spline/bspline-derv.html    */

BSpline::BSpline() {}
BSpline::~BSpline() {}

// ------ Parameterization ------

/*	Description
	-----------
	Produces an array of lengths (of the current size), each corresponding to a uniform increment of the natural parameter
	- These values are used as the basis for approximating certain parameterization functions
	- Used when a mapping between arc-length values and natural parameters is required

	Args
	----
	degree = Degree of piecewise polynomials which constitute the B-spline
	knots = Knot vector corresponding to the given control points (it must be increasing in value, normalized and uniformly spaced)    */
void BSpline::computeLengths(unsigned int degree, const std::vector<double>& knots, const std::vector<MVector>& controlPoints, 
	std::vector<double>& outlengths, std::vector<MVector>* outPoints)
{
	unsigned int sampleCount = (int)outlengths.size();
	assert(sampleCount >= 2);

	if (outPoints)
		outPoints->resize(sampleCount);

	unsigned int n = (unsigned)controlPoints.size() - 1;
	double lowerBound = knots[degree];
	double upperBound = knots[n + 1];

	// Cache arc-lengths by uniformly incrementing over the curves natural parameter t
	double step = (upperBound - lowerBound) / (double)(sampleCount - 1);
	double t = lowerBound;
	double tLength = 0.0;
	outlengths[0] = 0.0;
	MVector previousPoint = sampleCurve(lowerBound, degree, knots, controlPoints);
	if (outPoints)
		(*outPoints)[0] = previousPoint;

	for (unsigned int i = 1; i < sampleCount; i++)
	{
		t += step;

		MVector currentPoint = sampleCurve(t, degree, knots, controlPoints);
		MVector diff = previousPoint - currentPoint;
		if (outPoints)
			(*outPoints)[i] = currentPoint;

		tLength += diff.length();
		outlengths[i] = tLength;
		previousPoint = currentPoint;
	}
}

// ------ Knots ------

/*	Description
	-----------
	Computes a clamped knot vector for the given constants (it will allow the B-spline to pass through its end points)
	The knot vector will have a normalized domain [0, 1]
	There will be (n - degree + 2) internal knots over which the curve is defined, representing the entire domain of the knot vector
	There will be an extra (2 * degree) extended knots such that each end of the knot vector has (degree + 1) knots which share the same value
	There will be a total of (n + degree + 2) knots

	Considerations
	--------------
	The curve is defined over the knot intervals [degree, n] whereby a single interval i spans the knot vector over indices [i, i + 1)
	- It is very important to note that the end of each interval is exclusive
	- When determining a knot interval for the curve's parameter we must be careful not to exceed n
	- Specifically when the parameter is equal to the curve's upper domain limit (in this case a value of 1.0)
	
	Example
	-------
	For a B-spline of degree = 3, with 7 control points (ie. n = 6), the following knot vector will be produced :
	<0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1>
	This curve is defined over knot intervals [3, 6] -> [0.0, 1.0)
	
	Args
	----
	n =	Common constant used in spline calculations, n = number of control points - 1
	degree = Degree of piecewise polynomials which constitute the B-spline    */
std::vector<double> BSpline::computeClampedKnotVector(unsigned int n, unsigned int degree)
{
	unsigned int numOfInternalKnots = n - degree + 2;
	unsigned int numOfTotalKnots = n + degree + 2;

	std::vector<double> knotVector;
	knotVector.resize(numOfTotalKnots);
	
	for (unsigned int i = 0; i < degree; i++)
		knotVector[i] = 0.0;
	for (unsigned int i = degree; i < n + 2; i++)
		knotVector[i] = (double)(i - degree) / (numOfInternalKnots - 1);
	for (unsigned int i = n + 2; i < numOfTotalKnots; i++)
		knotVector[i] = 1.0;
		
	return knotVector;
}

/*	Description
	-----------
	Computes an unclamped knot vector for the given constants (the B-spline will not pass through its end points)
	Because the B-spline is not clamped, there will not be a uniform number of basis functions contributing to the calculation of each segment
	The knot vector will have a normalized domain [0, 1]
	There will be (n - degree + 2) knots over which the curve is defined, representing a subdomain of the knot vector
	There will be a total of (n + degree + 2) knots
	
	Considerations
	--------------
	The curve is defined over the knot intervals [degree, n] whereby a single interval i spans the knot vector over indices [i, i + 1)
	- It is very important to note that the end of each interval is exclusive
	- When determining a knot interval for the curve's parameter we must be careful not to exceed n
	- Specifically when the parameter is equal to the curve's upper domain limit

	Example
	-------
	For a B-spline of degree = 3, with 7 control points (ie. n = 6), the following knot vector will be produced :
	<0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1>
	This curve is defined over knot intervals [3, 6] -> [0.3, 0.7)

	Args
	----
	n =	Common constant used in spline calculations, n = number of control points - 1
	degree = Degree of piecewise polynomials which constitute the B-spline    */
std::vector<double> BSpline::computeUnclampedKnotVector(unsigned int n, unsigned int degree)
{
	unsigned int numOfKnots = n + degree + 2;

	std::vector<double> knotVector;
	knotVector.resize(numOfKnots);

	for (unsigned int i = 0; i < numOfKnots; ++i)
		knotVector[i] = (double)i / (numOfKnots - 1);

	return knotVector;
}

/*	Description
	-----------
	Determines the internal knot interval that contains the parameter t for any clamped/unclamped B-spline

	Considerations
	--------------
	The curve is defined over the knot intervals [degree, n] whereby a single interval i spans the knot vector over indices [i, i + 1)
	- It is very important to note that the end of each interval is exclusive
	- When determining a knot interval for the curve's parameter we must be careful not to exceed n
	- Specifically when the parameter is equal to the curve's upper domain limit

	Args
	----
	t = Natural parameter of the B-spline
	degree = Degree of piecewise polynomials which constitute the B-spline
	knots = Increasing value knot vector which contains the given value t    */
unsigned int BSpline::getKnotInterval(double t, unsigned int degree, const std::vector<double>& knots)
{
	// If the loop never breaks, the value of t is on the far boundary of the last internal interval
	unsigned int interval = (int)knots.size() - degree - 2;

	for (unsigned int i = degree; i <= interval; i++)
	{
		if (t >= knots[i] && t < knots[i + 1])
		{
			interval = i;
			break;
		}
	}

	return interval;
}

// ------ Sample ------

/*	Description
	-----------	
	This function is for reference, it shows an unoptimised implementation of the Carl de Boor algorithm used for generating B-splines    */
MVector BSpline::sampleCurve_DEPRECATED(double t, unsigned int degree, const std::vector<double>& knots, const std::vector<MVector>& controlPoints)
{
	unsigned int interval = getKnotInterval(t, degree, knots);

	// Cache the control points which will be needed to compute the basis functions [interval - degree, interval]
	// This range is determined by the recursive function (see triangle schematic)
	// ie. For a value t in a specific knot interval, there are (degree + 1) contributing basis functions used to calculate the point
	std::vector<MVector> controlPointsPrevious;
	controlPointsPrevious.resize(degree + 1);
	unsigned int controlPointIndex = 0;

	for (unsigned int i = interval - degree; i <= interval; i++)
	{
		controlPointsPrevious[controlPointIndex] = controlPoints[i];
		controlPointIndex++;
	}

	// Recursive function iterates over a variable r with range [1, p]
	// This is one less iteration than the standard formula which has (degree + 1) iterations
	// This is because r = 0 is represented above by the exact positions of the existing control points
	// For each iteration of r, we will find a barycentric point between consecutive pairs of previous temporary control points
	// When the recursion reaches the final iteration (ie. r = degree), we have found the point on the curve
	std::vector<MVector> controlPointsCurrent;
	double alpha;

	for (unsigned int r = 1; r <= degree; r++)
	{
		// For each iteration of r, there is one less temporary point (ie. an n-sized array will output n-1 barycentric points)
		controlPointsCurrent.resize(degree + 1 - r);
		controlPointIndex = 0;
		
		// The value of i is in relation to the original arrays and must be computed so we can access the correct knots
		// However our temporary control point arrays are zero-indexed (therefore a different indexing method is used)
		for (unsigned int i = interval - degree + r; i <= interval; i++)
		{
			alpha = (t - knots[i]) / (knots[i + 1 + degree - r] - knots[i]);
			controlPointsCurrent[controlPointIndex] = (1.0 - alpha) * controlPointsPrevious[controlPointIndex] + alpha * controlPointsPrevious[controlPointIndex + 1];
			controlPointIndex++;
		}

		controlPointsPrevious = controlPointsCurrent;
	}

	return controlPointsPrevious[0];
}

/*	Description
	-----------
	This is an optimised implementation of the Carl de Boor algorithm used for generating B-splines
	It expects a knot vector whose internal knots have increasing value, it does not handle repeating internal knots
	If an internal knot is repeated, the denominator of alpha (see below) will likely result in division by zero
	
	The main optimisation is as follows :
	- We can recognise that for each iteration of r, we are assigning new memory to store a new temporary array of points (one less than the previous iteration)
	- Each iteration of the inner loop i accesses a sequential pair of points from the previous iteration of r
	- eg. i : (point0, point1), i+1 : (point1, point2), i+2 : (point2, point3)
	- Meaning an arbitrary point from the previous iteration can be accessed twice during the current iteration of r
	- It is evident that once the first point in each pair has been accessed, that memory is no longer needed and can be overwritten
	
	Args
	----
	t = Natural parameter of the B-spline
	degree = Degree of piecewise polynomials which constitute the B-spline
	interval = Index of the internal knot interval which contains t ( eg. knots[interval] <= t < knots[interval + 1] )
	knots = Increasing value knot vector which contains the given value t and corresponds to the given control points
	controlPoints = Control points used to define the curve    */
MVector BSpline::sampleCurve(double t, unsigned int degree, const std::vector<double>& knots, const std::vector<MVector>& controlPoints)
{
	unsigned int interval = getKnotInterval(t, degree, knots);

	// These are the points for r = 0
	std::vector<MVector> controlPointsTemporary;
	controlPointsTemporary.resize(degree + 1);
	unsigned int controlPointIndex = 0;

	for (unsigned int i = interval - degree; i <= interval; i++)
	{
		controlPointsTemporary[controlPointIndex] = controlPoints[i];
		controlPointIndex++;
	}
	
	double alpha;

	for (unsigned int r = 1; r <= degree; r++)
	{
		controlPointIndex = 0;

		for (unsigned int i = interval - degree + r; i <= interval; i++)
		{
			alpha = (t - knots[i]) / (knots[i + 1 + degree - r] - knots[i]);
			controlPointsTemporary[controlPointIndex] = (1.0 - alpha) * controlPointsTemporary[controlPointIndex] + alpha * controlPointsTemporary[controlPointIndex + 1];
			controlPointIndex++;
		}
	}

	return controlPointsTemporary[0];
}

/*	Description
	-----------
	This function uses the fact that the first derivative of a B-spline is another B-spline whose degree is one less
	The B-spline uses the same knot vector and a modified set of control points (coefficients)
	- ie. When r = 0, we generate this set of control points using a difference equation
	- Thereafter, each recursive basis function generates a set of temporary points using the previous set (each new set is one smaller than the last)
	The above resource shows a function which iterates over n (number of control points - 1) basis functions
	- Using our knowledge of the Carl de Boor algorithm, we will reduce this equation to the range [interval - degree, interval - order]
	- The last iteration reflects the fact that the degree has reduced by the order of the derivative

	This function expects a knot vector whose internal knots have increasing value, it does not handle repeating internal knots
	If an internal knot is repeated, the denominator of alpha (see below) will likely result in division by zero

	Args
	----
	order = The order of the derivative to calculate, must be greater than zero and less than the continuity of the curve
	t = Natural parameter of the B-spline
	degree = Degree of piecewise polynomials which constitute the B-spline
	interval = Index of the internal knot interval which contains t ( eg. knots[interval] <= t < knots[interval + 1] )
	knots = Increasing value knot vector which contains the given value t and corresponds to the given control points
	controlPoints = Control points used to define the curve    */
MVector BSpline::sampleDerivative(unsigned int order, double t, unsigned int degree, const std::vector<double>& knots,
	const std::vector<MVector>& controlPoints)
{
	// Continuity = order of curve - 2 (the nth derivative can only be taken if the curve has equivalent continuity)
	assert(degree + 1 - 2 >= order);
	assert(order > 0);

	unsigned int interval = getKnotInterval(t, degree, knots);

	// These are the points for r = 0
	std::vector<MVector> controlPointsTemporary;
	controlPointsTemporary.resize(degree);
	unsigned int controlPointIndex = 0;

	// The difference equation is applied to points for the first derivative
	for (unsigned int i = interval - degree; i <= interval - 1; i++)
	{
		controlPointsTemporary[controlPointIndex] = degree / (knots[i + degree + 1] - knots[i + 1]) * (controlPoints[i + 1] - controlPoints[i]);
		controlPointIndex++;
	}

	// If a higher order derivative is to be taken then we need to recursively apply the difference equation for each higher derivative
	for (unsigned int j = 1; j <= order - 1; j++)
	{
		controlPointIndex = 0;

		for (unsigned int i = interval - degree; i <= interval - 1 - j; i++)
		{
			controlPointsTemporary[controlPointIndex] = degree / (knots[i + degree + 1] - knots[i + 1]) * (controlPointsTemporary[controlPointIndex + 1] - controlPointsTemporary[controlPointIndex]);
			controlPointIndex++;
		}
	}

	double alpha;

	for (unsigned int r = 1; r <= degree - order; r++)
	{
		controlPointIndex = 0;

		for (unsigned int i = interval - degree + r; i <= interval - order; i++)
		{
			alpha = (t - knots[i + order]) / (knots[i + 1 + degree - r] - knots[i + order]);
			controlPointsTemporary[controlPointIndex] = (1.0 - alpha) * controlPointsTemporary[controlPointIndex] + alpha * controlPointsTemporary[controlPointIndex + 1];
			controlPointIndex++;
		}
	}

	return controlPointsTemporary[0];
}

/*	Description
	-----------
	Provides a value for the curvature at a given value of the natural parameter t

	The calculation for curvature is synonymous with the rate at which the tangent changes in relation to the arc-length of the curve
	The calculation uses the gemometric properties of a curve and is therefore relative to the size of that curve
	In general the value of an arbitrary sample should be normalized in relation to a sequence of other samples

	Args
	----
	t = Natural parameter of the curve
	degree = Degree of piecewise polynomials which constitute the B-spline
	interval = Index of the internal knot interval which contains t ( eg. knots[interval] <= t < knots[interval + 1] )
	knots = Increasing value knot vector which contains the given value t and corresponds to the given control points
	controlPoints = Control points used to define the curve    */
double BSpline::sampleCurvature(double t, unsigned int degree, const std::vector<double>& knots, const std::vector<MVector>& controlPoints)
{
	unsigned int interval = getKnotInterval(t, degree, knots);

	MVector vFirstDerivative = sampleDerivative(1, t, degree, knots, controlPoints);
	MVector vSecondDerivative = sampleDerivative(2, t, degree, knots, controlPoints);
	double firstLength = vFirstDerivative.length();

	return (vFirstDerivative ^ vSecondDerivative).length() / (firstLength * firstLength * firstLength);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------