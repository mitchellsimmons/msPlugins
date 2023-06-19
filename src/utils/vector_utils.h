#pragma once

#include <vector>
#include <cassert>

#include <maya/MVector.h>
#include <maya/MQuaternion.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ------ Helpers -------------------------------------------------------------------------------------------------------------------------------------------------------------

MVector averageVector(const std::vector<MVector>& vectors);
// Function assumes the size of each input array is equal, else behaviour is undefined
MVector averageWeightedVector(const std::vector<MVector>& vectors, const std::vector<double>& weights);

// Optimization function for applying a unit quaternion rotation to a vector
MVector rotateVectorByQuaternion(const MVector& v, const MQuaternion& q);
// Rotates a vector around a given axis by a given angle (expects a unit vector for the axis)
MVector rotateVectorByAxisAngle(const MVector& v, const MVector& axis, double angle);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------