// Contains a set of functions relating to quaternion operations

#include <cassert>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>

#include <maya/MQuaternion.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// The following two functions provide an option for multiplying quaternions in standard order (ie. rhs is applied relative to lhs)
// Maya uses the opposite order which is representative of how it handles matrix multiplication
MQuaternion quaternionPureMultiply(const MQuaternion& q1, const MQuaternion& q2);
MQuaternion quaternionMultiply(const MQuaternion& q1, const MQuaternion& q2);

// The following two functions provide an approximate average between multiple rotations
// When two quaternions with equal weights are given, the result is equivalent to a slerp with parameter 0.5
// Based upon the following resources:
// https://stackoverflow.com/questions/12374087/average-of-multiple-quaternions
// https://github.com/fizyr/dr_eigen/blob/master/include/dr_eigen/average.hpp
MQuaternion averageQuaternion(const std::vector<MQuaternion>& quaternions);
// Function assumes the size of each input array is equal, else behaviour is undefined
MQuaternion averageWeightedQuaternion(const std::vector<MQuaternion>& quaternions, const std::vector<double>& weights);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------