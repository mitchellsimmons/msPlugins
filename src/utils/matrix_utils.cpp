#include "matrix_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ------ Helpers -------------------------------------------------------------------------------------------------------------------------------------------------------------

MMatrix matrixFromVectors(const MVector& x, const MVector& y, const MVector& z, const MVector& p)
{
	MMatrix ret;
	ret[0][0] = x.x; ret[0][1] = x.y; ret[0][2] = x.z;
	ret[1][0] = y.x; ret[1][1] = y.y; ret[1][2] = y.z;
	ret[2][0] = z.x; ret[2][1] = z.y; ret[2][2] = z.z;
	ret[3][0] = p.x; ret[3][1] = p.y; ret[3][2] = p.z;

	return ret;
}

MFloatMatrix matrixFromVectors(const MFloatVector& x, const MFloatVector& y, const MFloatVector& z, const MFloatVector& p)
{
	MFloatMatrix ret;
	ret[0][0] = x.x; ret[0][1] = x.y; ret[0][2] = x.z;
	ret[1][0] = y.x; ret[1][1] = y.y; ret[1][2] = y.z;
	ret[2][0] = z.x; ret[2][1] = z.y; ret[2][2] = z.z;
	ret[3][0] = p.x; ret[3][1] = p.y; ret[3][2] = p.z;

	return ret;
}

MMatrix composeMatrix(const MVector& translation, const MQuaternion& rotation, const MVector& scale)
{
	MTransformationMatrix transform;
	transform.setTranslation(translation, MSpace::kTransform);
	transform.setRotationQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
	transform.setScale(&scale.x, MSpace::kTransform);
	return transform.asMatrix();
}

MMatrix composeMatrix(const MVector& translation, const MEulerRotation& rotation, const MVector& scale)
{
	MTransformationMatrix transform;
	transform.setTranslation(translation, MSpace::kTransform);
	transform.setRotation(&rotation.x, (MTransformationMatrix::RotationOrder)(rotation.order + 1));
	transform.setScale(&scale.x, MSpace::kTransform);
	return transform.asMatrix();
}

// When all transformations are required, decomposing as a single operation is most efficient
void decomposeMatrix(const MMatrix& matrix, MVector& outTranslation, MQuaternion& outRotation, MVector& outScale)
{
	outTranslation = extractTranslation(matrix);
	outScale = extractScale(matrix);

	// By manually extracting the rotation, we can avoid repeating the scaling ops which were just completed
	// MTransformationMatrix::rotation() would need to recompute the scale in order to normalize the basis as below
	MMatrix rotMatrix;
	if (!MRS::isEqual(outScale.x, 0.0))
	{
		double mult = 1.0 / outScale.x;
		rotMatrix[0][0] = matrix[0][0] * mult;
		rotMatrix[0][1] = matrix[0][1] * mult;
		rotMatrix[0][2] = matrix[0][2] * mult;
	}
	else
		rotMatrix[0][0] = 0.0;

	if (!MRS::isEqual(outScale.y, 0.0))
	{
		double mult = 1.0 / outScale.y;
		rotMatrix[1][0] = matrix[1][0] * mult;
		rotMatrix[1][1] = matrix[1][1] * mult;
		rotMatrix[1][2] = matrix[1][2] * mult;
	}
	else
		rotMatrix[1][1] = 0.0;

	if (!MRS::isEqual(outScale.z, 0.0))
	{
		double mult = 1.0 / outScale.z;
		rotMatrix[2][0] = matrix[2][0] * mult;
		rotMatrix[2][1] = matrix[2][1] * mult;
		rotMatrix[2][2] = matrix[2][2] * mult;
	}
	else
		rotMatrix[2][2] = 0.0;

	outRotation.w = std::sqrt(std::max(0.0, 1.0 + rotMatrix[0][0] + rotMatrix[1][1] + rotMatrix[2][2])) * 0.5;
	outRotation.x = std::sqrt(std::max(0.0, 1.0 + rotMatrix[0][0] - rotMatrix[1][1] - rotMatrix[2][2])) * 0.5;
	outRotation.y = std::sqrt(std::max(0.0, 1.0 - rotMatrix[0][0] + rotMatrix[1][1] - rotMatrix[2][2])) * 0.5;
	outRotation.z = std::sqrt(std::max(0.0, 1.0 - rotMatrix[0][0] - rotMatrix[1][1] + rotMatrix[2][2])) * 0.5;
	outRotation.x = std::copysign(outRotation.x, -1 * (rotMatrix[2][1] - rotMatrix[1][2]));
	outRotation.y = std::copysign(outRotation.y, -1 * (rotMatrix[0][2] - rotMatrix[2][0]));
	outRotation.z = std::copysign(outRotation.z, -1 * (rotMatrix[1][0] - rotMatrix[0][1]));
}

void decomposeMatrix(const MMatrix& matrix, MEulerRotation::RotationOrder rotationOrder, MVector& outTranslation, MEulerRotation& outRotation, MVector& outScale)
{
	outTranslation = extractTranslation(matrix);
	outScale = extractScale(matrix);

	MMatrix rotMatrix;
	if (!MRS::isEqual(outScale.x, 0.0))
	{
		double mult = 1.0 / outScale.x;
		rotMatrix[0][0] = matrix[0][0] * mult;
		rotMatrix[0][1] = matrix[0][1] * mult;
		rotMatrix[0][2] = matrix[0][2] * mult;
	}
	else
		rotMatrix[0][0] = 0.0;

	if (!MRS::isEqual(outScale.y, 0.0))
	{
		double mult = 1.0 / outScale.y;
		rotMatrix[1][0] = matrix[1][0] * mult;
		rotMatrix[1][1] = matrix[1][1] * mult;
		rotMatrix[1][2] = matrix[1][2] * mult;
	}
	else
		rotMatrix[1][1] = 0.0;

	if (!MRS::isEqual(outScale.z, 0.0))
	{
		double mult = 1.0 / outScale.z;
		rotMatrix[2][0] = matrix[2][0] * mult;
		rotMatrix[2][1] = matrix[2][1] * mult;
		rotMatrix[2][2] = matrix[2][2] * mult;
	}
	else
		rotMatrix[2][2] = 0.0;

	outRotation = MEulerRotation::decompose(rotMatrix, rotationOrder);
}

// Rotational matrix is orthonormal, therefore scale is determined by the length of each vector in the composition
// Negative scaling in any single axis will always be extracted into the z-axis
// Negative scaling in any two axes will be treated as a rotation around the third axis
MMatrix extractScaleMatrix(const MMatrix& matrix)
{
	double scaleX = std::sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
	double scaleY = std::sqrt(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
	double scaleZ = std::sqrt(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);
	
	// The determinant tests the handedness of our basis: (x ^ y) * z < 0
	// If it is negative, there is an odd number of reflections (ie. one axis has a negative scale)
	// If it is positive, there are either no reflections or 2 reflections (ie. 2 axes may have a negative scale)
	// However two reflections is the same as a 180 degree rotation around the third axis (it is impossible to extract the correct information)
	// Therefore we only handle the single reflection case by negating the z-axis (this is how the API handles it)
	if (matrix.det3x3() < 0.0)
		scaleZ *= -1;

	MMatrix ret;
	ret[0][0] = scaleX; ret[1][1] = scaleY; ret[2][2] = scaleZ;
	return ret;
}

MFloatMatrix extractScaleMatrix(const MFloatMatrix& matrix)
{
	float scaleX = std::sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
	float scaleY = std::sqrt(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
	float scaleZ = std::sqrt(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);

	if (matrix.det3x3() < 0.0)
		scaleZ *= -1;

	MFloatMatrix ret;
	ret[0][0] = scaleX; ret[1][1] = scaleY; ret[2][2] = scaleZ;
	return ret;
}

MVector extractScale(const MMatrix& matrix)
{
	double scaleX = std::sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
	double scaleY = std::sqrt(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
	double scaleZ = std::sqrt(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);

	if (matrix.det3x3() < 0.0)
		scaleZ *= -1;

	return { scaleX, scaleY, scaleZ };
}

MFloatVector extractScale(const MFloatMatrix& matrix)
{
	float scaleX = std::sqrt(matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2]);
	float scaleY = std::sqrt(matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2]);
	float scaleZ = std::sqrt(matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2]);

	if (matrix.det3x3() < 0.0)
		scaleZ *= -1;

	return { scaleX, scaleY, scaleZ };
}

// Returns a matrix which is the composition of all existing rotations (ie. rotation, jointOrient, rotationOrientation)
// Rotational matrix is orthonormal, therefore normalizing each basis in the composition will remove the effects of scaling
MMatrix extractRotationMatrix(const MMatrix& matrix)
{
	MMatrix rotMatrix;
	MVector scale = extractScale(matrix);

	if (!MRS::isEqual(scale.x, 0.0))
	{
		double mult = 1.0 / scale.x;
		rotMatrix[0][0] = matrix[0][0] * mult; 
		rotMatrix[0][1] = matrix[0][1] * mult;
		rotMatrix[0][2] = matrix[0][2] * mult;
	}
	else
		rotMatrix[0][0] = 0.0;

	if (!MRS::isEqual(scale.y, 0.0))
	{
		double mult = 1.0 / scale.y;
		rotMatrix[1][0] = matrix[1][0] * mult;
		rotMatrix[1][1] = matrix[1][1] * mult;
		rotMatrix[1][2] = matrix[1][2] * mult;
	}
	else
		rotMatrix[1][1] = 0.0;

	if (!MRS::isEqual(scale.z, 0.0))
	{
		double mult = 1.0 / scale.z;
		rotMatrix[2][0] = matrix[2][0] * mult;
		rotMatrix[2][1] = matrix[2][1] * mult;
		rotMatrix[2][2] = matrix[2][2] * mult;
	}
	else
		rotMatrix[2][2] = 0.0;

	return rotMatrix;
}

MFloatMatrix extractRotationMatrix(const MFloatMatrix& matrix)
{
	MFloatMatrix rotMatrix;
	MFloatVector scale = extractScale(matrix);

	if (!MRS::isEqual(scale.x, 0.0))
	{
		float mult = 1.0f / scale.x;
		rotMatrix[0][0] = matrix[0][0] * mult;
		rotMatrix[0][1] = matrix[0][1] * mult;
		rotMatrix[0][2] = matrix[0][2] * mult;
	}
	else
		rotMatrix[0][0] = 0.0;

	if (!MRS::isEqual(scale.y, 0.0))
	{
		float mult = 1.0f / scale.y;
		rotMatrix[1][0] = matrix[1][0] * mult;
		rotMatrix[1][1] = matrix[1][1] * mult;
		rotMatrix[1][2] = matrix[1][2] * mult;
	}
	else
		rotMatrix[1][1] = 0.0;

	if (!MRS::isEqual(scale.z, 0.0))
	{
		float mult = 1.0f / scale.z;
		rotMatrix[2][0] = matrix[2][0] * mult;
		rotMatrix[2][1] = matrix[2][1] * mult;
		rotMatrix[2][2] = matrix[2][2] * mult;
	}
	else
		rotMatrix[2][2] = 0.0;

	return rotMatrix;
}

// Returns an euler rotation which is the composition of all existing rotations (ie. rotation, jointOrient, rotationOrientation)
MEulerRotation extractEulerRotation(const MMatrix& matrix, MEulerRotation::RotationOrder rotationOrder)
{
	// NOTE : Maya's API provides two ways we can extract rotation
	// - The MEulerRotation::decompose() method does not account for non-uniform scaling, we must provide it a normalized rotation matrix
	// - The MTransformationMatrix::eulerRotation() method accounts for non-uniform scaling, we must then call MEulerRotation::reorderIt() on the result
	MMatrix rotMatrix = extractRotationMatrix(matrix);
	return MEulerRotation::decompose(rotMatrix, rotationOrder);
}

// Returns a quaternion which can be used to rotate a vector
// Rotation conversion: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
MQuaternion extractQuaternionRotation(const MMatrix& matrix)
{
	MQuaternion output;
	MMatrix rotMatrix = extractRotationMatrix(matrix);

	output.w = std::sqrt(std::max(0.0, 1.0 + rotMatrix[0][0] + rotMatrix[1][1] + rotMatrix[2][2])) * 0.5;
	output.x = std::sqrt(std::max(0.0, 1.0 + rotMatrix[0][0] - rotMatrix[1][1] - rotMatrix[2][2])) * 0.5;
	output.y = std::sqrt(std::max(0.0, 1.0 - rotMatrix[0][0] + rotMatrix[1][1] - rotMatrix[2][2])) * 0.5;
	output.z = std::sqrt(std::max(0.0, 1.0 - rotMatrix[0][0] - rotMatrix[1][1] + rotMatrix[2][2])) * 0.5;
	output.x = std::copysign(output.x, -1 * (rotMatrix[2][1] - rotMatrix[1][2]));
	output.y = std::copysign(output.y, -1 * (rotMatrix[0][2] - rotMatrix[2][0]));
	output.z = std::copysign(output.z, -1 * (rotMatrix[1][0] - rotMatrix[0][1]));

	return output;
}

MMatrix extractTranslationMatrix(const MMatrix& matrix, MSpace::Space space)
{
	MMatrix ret;
	if (space == MSpace::kPreTransform)
	{
		MTransformationMatrix fnTransMat{ matrix };
		MVector translation = fnTransMat.getTranslation(MSpace::kPreTransform);
		ret[3][0] = translation[0]; ret[3][1] = translation[1]; ret[3][2] = translation[2];
	}
	else
		ret[3][0] = matrix[3][0]; ret[3][1] = matrix[3][1]; ret[3][2] = matrix[3][2];

	return ret;
}

MFloatMatrix extractTranslationMatrix(const MFloatMatrix& matrix, MSpace::Space space)
{
	MFloatMatrix ret;
	if (space == MSpace::kPreTransform)
	{
		MMatrix convertedMatrix;
		matrix.get(convertedMatrix.matrix);
		MTransformationMatrix fnTransMat{ convertedMatrix };
		MFloatVector translation = fnTransMat.getTranslation(MSpace::kPreTransform);
		ret[3][0] = translation[0]; ret[3][1] = translation[1]; ret[3][2] = translation[2];
	}
	else
		ret[3][0] = matrix[3][0]; ret[3][1] = matrix[3][1]; ret[3][2] = matrix[3][2];

	return ret;
}

MVector extractTranslation(const MMatrix& matrix, MSpace::Space space)
{
	if (space == MSpace::kPreTransform)
	{
		MTransformationMatrix fnTransMat{ matrix };
		return fnTransMat.getTranslation(MSpace::kPreTransform);
	}
	else
		return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

MFloatVector extractTranslation(const MFloatMatrix& matrix, MSpace::Space space)
{
	if (space == MSpace::kPreTransform)
	{
		MMatrix doubleMatrix;
		matrix.get(doubleMatrix.matrix);
		MTransformationMatrix fnTransMat{ doubleMatrix };
		return fnTransMat.getTranslation(MSpace::kPreTransform);
	}
	else
		return { matrix[3][0], matrix[3][1], matrix[3][2] };
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------