#include "quaternion_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// This is an optimised function for multiplying two pure quaternions (should be around half the operations of a normal mult)
// [0,a]*[0,b] = [-a dot b, a cross b]
MQuaternion quaternionPureMultiply(const MQuaternion& q1, const MQuaternion& q2)
{
	MQuaternion result;
	result.w = -(q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
	result.x = (q1.y * q2.z) - (q2.y * q1.z);
	result.y = (q1.z * q2.x) - (q2.z * q1.x);
	result.z = (q1.x * q2.y) - (q2.x * q1.y);
	return result;
}

MQuaternion quaternionMultiply(const MQuaternion& q1, const MQuaternion& q2) {
	MQuaternion result;
	result.w = -(q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z) + (q1.w * q2.w);
	result.x = (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y) + (q1.w * q2.x);
	result.y = -(q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x) + (q1.w * q2.y);
	result.z = (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w) + (q1.w * q2.z);
	return result;
}

MQuaternion averageQuaternion(const std::vector<MQuaternion>& quaternions)
{
	size_t count = quaternions.size();
	if (count == 0)
		return MQuaternion::identity;

	Eigen::Matrix<double, 4, 4> A = Eigen::Matrix<double, 4, 4>::Zero();
	for (const MQuaternion& input : quaternions)
	{
		Eigen::Matrix<double, 1, 4> q(1, 4);
		q(0) = input.w;
		q(1) = input.x;
		q(2) = input.y;
		q(3) = input.z;
		A += q.transpose()*q;
	}
	A /= (double)count;

	Eigen::EigenSolver<Eigen::Matrix<double, 4, 4>> es{ A };
	Eigen::Matrix<std::complex<double>, 4, 1> mat{ es.eigenvalues() };
	int maxEigenValueIndex;
	mat.real().maxCoeff(&maxEigenValueIndex);
	Eigen::Matrix<double, 4, 1> maxEigenVec{ es.eigenvectors().real().block(0, maxEigenValueIndex, 4, 1) };

	return MQuaternion{ maxEigenVec(1), maxEigenVec(2), maxEigenVec(3), maxEigenVec(0) };
}

MQuaternion averageWeightedQuaternion(const std::vector<MQuaternion>& quaternions, const std::vector<double>& weights)
{
	size_t count = quaternions.size();
	if (count == 0)
		return MQuaternion::identity;
	assert(count == weights.size());

	double weightSum = 0.0;
	Eigen::Matrix<double, 4, 4> A = Eigen::Matrix<double, 4, 4>::Zero();
	for (unsigned int i = 0; i < count; ++i)
	{
		const MQuaternion& input = quaternions[i];
		const double& weight = weights[i];
		Eigen::Matrix<double, 1, 4> q(1, 4);
		q(0) = input.w;
		q(1) = input.x;
		q(2) = input.y;
		q(3) = input.z;
		A += weight * (q.transpose()*q);
		weightSum += weight;
	}

	if (weightSum != 0.0)
		A /= weightSum;

	Eigen::EigenSolver<Eigen::Matrix<double, 4, 4>> es{ A };
	Eigen::Matrix<std::complex<double>, 4, 1> mat{ es.eigenvalues() };
	int maxEigenValueIndex;
	mat.real().maxCoeff(&maxEigenValueIndex);
	Eigen::Matrix<double, 4, 1> maxEigenVec{ es.eigenvectors().real().block(0, maxEigenValueIndex, 4, 1) };

	return MQuaternion{ maxEigenVec(1), maxEigenVec(2), maxEigenVec(3), maxEigenVec(0) };
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------