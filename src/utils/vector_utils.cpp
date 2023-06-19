#include "vector_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ------ Helpers -------------------------------------------------------------------------------------------------------------------------------------------------------------

MVector averageVector(const std::vector<MVector>& vectors)
{
	size_t count = vectors.size();

	MVector vSum = MVector::zero;
	for (const MVector& vector : vectors)
		vSum += vector;

	return count == 0 ? vSum : vSum / (double)count;
}

MVector averageWeightedVector(const std::vector<MVector>& vectors, const std::vector<double>& weights)
{
	size_t count = vectors.size();
	assert(count == weights.size());

	double weightSum = 0.0;
	MVector vSum = MVector::zero;
	for (unsigned int i = 0; i < count; ++i)
	{
		vSum += vectors[i] * weights[i];
		weightSum += weights[i];
	}

	return weightSum == 0.0 ? vSum : vSum / weightSum;
}

MVector rotateVectorByQuaternion(const MVector& v, const MQuaternion& q)
{
	MVector u{ q.x, q.y, q.z };
	return 2.0 * (u * v) * u + (q.w * q.w - (u * u)) * v + 2.0 * q.w * (u ^ v);
}

MVector rotateVectorByAxisAngle(const MVector& v, const MVector& axis, double angle)
{
	double x = cos(angle);
	double y = sin(angle);
	return (1 - x) * (v * axis) * axis + x * v + y * (axis ^ v);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------