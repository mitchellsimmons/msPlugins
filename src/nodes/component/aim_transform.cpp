#include "aim_transform.h"

/*	Description
	-----------
	A custom transform that encompasses the basic functionality of an aim constraint.
	Input axes and world space positions define a world space basis frame which is then localised using the parentInverseFrame.

	Attributes
	----------
	parentInverseFrame - matrix
		The world inverse frame of this node's parent transform.
		This frame is used to localise the resulting world space frame into this transform's parent space.

	rootPosition - matrix
		Specifies the world position that this transform should inherit.

	aimPosition - matrix
		Specifies the world position that this transform's `aimAxis` should aim towards.

	upPosition - matrix
		Specifies the world position that this transform's `upAxis` should attempt to aim towards.

	aimAxis - enum
		Specifies the axis that this transform should use to aim at the `aimPosition`.

		+x (0)
			The positive direction of the x-axis.
		+y (1)
			The positive direction of the y-axis.
		+z (2)
			The positive direction of the z-axis.
		-x (3)
			The negative direction of the x-axis.
		-y (4)
			The negative direction of the y-axis.
		-z (5)
			The negative direction of the z-axis.

	upAxis - enum
		Specifies the axis that this transform should use to aim at the `upPosition`.
		This axis will be automatically resolved if it conflicts with the `aimAxis`.

		+x (0)
			The positive direction of the x-axis.
		+y (1)
			The positive direction of the y-axis.
		+z (2)
			The positive direction of the z-axis.
		-x (3)
			The negative direction of the x-axis.
		-y (4)
			The negative direction of the y-axis.
		-z (5)
			The negative direction of the z-axis.
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AimMatrix::AimMatrix() : MPxTransformationMatrix(), aimAxis{ kPosX }, upAxis{ kPosY } {};
AimMatrix::~AimMatrix() {};

// ------ MPxTransformationMatrix ------
MMatrix AimMatrix::asMatrix() const
{
	return matrixFromInternals();
}

MMatrix AimMatrix::asMatrix(double percent) const
{
	return matrixFromInternals();
}

/*	Description
	-----------
	Calculates the local matrix using internally cached input data from the associated MPxTransform
	It is the responsibility of MPxTransform::validateAndSetValue() to retrieve this data during DG evaluation
	It is the responsibility of MPxTransform::compute() to retrieve this data during parallel evaluation    */
MMatrix AimMatrix::matrixFromInternals() const
{
	// Using double cross product technique to build an orthonormal transform
	MVector aimVector = aimPosition - rootPosition;
	aimVector.normalize();

	MVector binormalVector = aimVector ^ (upPosition - rootPosition);
	binormalVector.normalize();

	MVector normalVector = binormalVector ^ aimVector;

	MRS::Matrix33<double> orientFrame{ aimVector, normalVector, binormalVector };

	// Resolve upAxis conflicts
	Axis upAxisResolved = upAxis;

	switch (aimAxis)
	{
		// upAxis defaults to (+y) if it conflicts with aimAxis (+x, -x)
		case kPosX: case kNegX:
		{
			if (upAxis == kPosX || upAxis == kNegX)
				upAxisResolved = kPosY;

			break;
		}
		// upAxis defaults to (+x) if it conflicts with aimAxis (+y, -y)
		case kPosY: case kNegY:
		{
			if (upAxis == kPosY || upAxis == kNegY)
				upAxisResolved = kPosX;

			break;
		}
		// upAxis defaults to (+y) if it conflicts with aimAxis (+z, -z)
		case kPosZ: case kNegZ:
		{
			if (upAxis == kPosZ || upAxis == kNegZ)
				upAxisResolved = kPosY;

			break;
		}
	}

	// Re-orient the basis
	switch (aimAxis)
	{
		case kPosX:
		{
			switch (upAxisResolved)
			{
				case kPosZ:
				{
					orientFrame = orientFrame.preRotateInX(-M_PI_2);
					break;
				}
				case kNegY:
				{
					orientFrame = orientFrame.preRotateInX(M_PI);
					break;
				}
				case kNegZ:
				{
					orientFrame = orientFrame.preRotateInX(M_PI_2);
					break;
				}
			}
			break;
		}
		case kPosY:
		{
			switch (upAxisResolved)
			{
				case kPosX:
				{
					orientFrame = orientFrame.preRotateInX(M_PI);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kPosZ:
				{
					orientFrame = orientFrame.preRotateInX(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegX:
				{
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegZ:
				{
					orientFrame = orientFrame.preRotateInX(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
			}
			break;
		}
		case kPosZ:
		{
			switch (upAxisResolved)
			{
				case kPosX:
				{
					orientFrame = orientFrame.preRotateInY(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosY:
				{
					orientFrame = orientFrame.preRotateInY(M_PI_2);
					break;
				}
				case kNegX:
				{
					orientFrame = orientFrame.preRotateInY(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegY:
				{
					orientFrame = orientFrame.preRotateInY(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
		case kNegX:
		{
			switch (upAxisResolved)
			{
				case kPosY:
				{
					orientFrame = orientFrame.preRotateInY(M_PI);
					break;
				}
				case kPosZ:
				{
					orientFrame = orientFrame.preRotateInX(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI);
					break;
				}
				case kNegY:
				{
					orientFrame = orientFrame.preRotateInX(M_PI);
					orientFrame = orientFrame.preRotateInY(M_PI);
					break;
				}
				case kNegZ:
				{
					orientFrame = orientFrame.preRotateInX(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
		case kNegY:
		{
			switch (upAxisResolved)
			{
				case kPosX:
				{
					orientFrame = orientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosZ:
				{
					orientFrame = orientFrame.preRotateInX(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kNegX:
				{
					orientFrame = orientFrame.preRotateInY(M_PI);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegZ:
				{
					orientFrame = orientFrame.preRotateInX(M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI_2);
					break;
				}
			}
			break;
		}
		case kNegZ:
		{
			switch (upAxisResolved)
			{
				case kPosX:
				{
					orientFrame = orientFrame.preRotateInY(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosY:
				{
					orientFrame = orientFrame.preRotateInY(-M_PI_2);
					break;
				}
				case kNegX:
				{
					orientFrame = orientFrame.preRotateInY(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegY:
				{
					orientFrame = orientFrame.preRotateInY(-M_PI_2);
					orientFrame = orientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
	}

	MMatrix outputFrame;
	orientFrame.get(outputFrame.matrix);
	outputFrame[3][0] = rootPosition[0]; outputFrame[3][1] = rootPosition[1]; outputFrame[3][2] = rootPosition[2];
	outputFrame *= parentInverseFrame;
	
	return outputFrame;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AimTransform::AimTransform() : MPxTransform(), NodeHelper() {}
#if MAYA_API_VERSION < 20190000
AimTransform::AimTransform(MPxTransformationMatrix* p_mtx) : MPxTransform(p_mtx), NodeHelper() {}
#endif
AimTransform::~AimTransform() {}

// ------ Attr ------
MObject AimTransform::parentInverseFrameAttr;
MObject AimTransform::rootPositionAttr;
MObject AimTransform::aimPositionAttr;
MObject AimTransform::upPositionAttr;
MObject AimTransform::aimAxisAttr;
MObject AimTransform::upAxisAttr;

// ------ MPxTransform ------
MPxTransform::SchedulingType AimTransform::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AimTransform::initialize()
{
	MMatrix defaultMatrix;
	std::unordered_map<const char*, short> fields{ {"+x", 0}, {"+y", 1}, {"+z", 2}, {"-x", 3}, {"-y", 4}, {"-z", 5} };
	createMatrixAttribute(parentInverseFrameAttr, "parentInverseFrame", "parentInverseFrame", defaultMatrix, kDefaultPreset | kAffectsWorldSpace);
	createMatrixAttribute(rootPositionAttr, "rootPosition", "rootPosition", defaultMatrix, kDefaultPreset | kAffectsWorldSpace);
	createMatrixAttribute(aimPositionAttr, "aimPosition", "aimPosition", defaultMatrix, kDefaultPreset | kAffectsWorldSpace);
	createMatrixAttribute(upPositionAttr, "upPosition", "upPosition", defaultMatrix, kDefaultPreset | kAffectsWorldSpace);
	createEnumAttribute(aimAxisAttr, "aimAxis", "aimAxis", fields, 0, kDefaultPreset | kKeyable | kAffectsWorldSpace);
	createEnumAttribute(upAxisAttr, "upAxis", "upAxis", fields, 1, kDefaultPreset | kKeyable | kAffectsWorldSpace);
	
	addAttribute(parentInverseFrameAttr);
	addAttribute(rootPositionAttr);
	addAttribute(aimPositionAttr);
	addAttribute(upPositionAttr);
	addAttribute(aimAxisAttr);
	addAttribute(upAxisAttr);
	
	attributeAffects(parentInverseFrameAttr, matrix);
	attributeAffects(rootPositionAttr, matrix);
	attributeAffects(aimPositionAttr, matrix);
	attributeAffects(upPositionAttr, matrix);
	attributeAffects(aimAxisAttr, matrix);
	attributeAffects(upAxisAttr, matrix);

	// Ensure input attributes produce calls to validateAndSetValue() during DG evaluation
	mustCallValidateAndSet(parentInverseFrameAttr);
	mustCallValidateAndSet(rootPositionAttr);
	mustCallValidateAndSet(aimPositionAttr);
	mustCallValidateAndSet(upPositionAttr);
	mustCallValidateAndSet(aimAxisAttr);
	mustCallValidateAndSet(upAxisAttr);
	
	return MStatus::kSuccess;
}

MPxTransformationMatrix* AimTransform::createTransformationMatrix() 
{
	return new AimMatrix();
}

/*	Description
	-----------
	Retrieves data from dirtied input attributes which have been flagged by mustCallValidateAndSet()
	When the Evaluation Graph is in use, this responsibility is deferred to compute() as calls to validateAndSetValue() will no longer be made
	This function is also responsible for writing the retrieved values into the MDataBlock to ensure they persist between calls    */
MStatus AimTransform::VALIDATE_AND_SET_VALUE_PROTOYPE
{
	AimMatrix* aimMatrix = static_cast<AimMatrix*>(transformationMatrixPtr());
	
	if (plug == rootPositionAttr)
	{
		// Cache value
		MMatrix rootFrame = handle.asMatrix();
		aimMatrix->rootPosition = MRS::extractTranslation(rootFrame, MSpace::kPostTransform);

		// Write value
		// This requirement is not discussed in the documentation however it is critical for any plug which is not connected upstream
		// If we do not write the value into the MDataBlock, any value which was set as a result of a user setAttr operation will be reset to the previous value
		// Attempting to set the value in the channelBox will make it seem like the attribute is locked as the value is immediately reset
		MDataBlock dataBlock = forceCache();
		outputMatrixValue(dataBlock, plug, rootFrame);
		
		return MStatus::kSuccess;
	}
	else if (plug == aimPositionAttr)
	{
		MMatrix aimFrame = handle.asMatrix();
		aimMatrix->aimPosition = MRS::extractTranslation(aimFrame, MSpace::kPostTransform);

		MDataBlock dataBlock = forceCache();
		outputMatrixValue(dataBlock, plug, aimFrame);
		
		return MStatus::kSuccess;
	}
	else if (plug == upPositionAttr)
	{
		MMatrix upVectorFrame = handle.asMatrix();
		aimMatrix->upPosition = MRS::extractTranslation(upVectorFrame, MSpace::kPostTransform);

		MDataBlock dataBlock = forceCache();
		outputMatrixValue(dataBlock, plug, upVectorFrame);

		return MStatus::kSuccess;
	}
	else if (plug == parentInverseFrameAttr)
	{
		MMatrix parentInverseFrame = handle.asMatrix();
		aimMatrix->parentInverseFrame = parentInverseFrame;

		MDataBlock dataBlock = forceCache();
		outputMatrixValue(dataBlock, plug, parentInverseFrame);

		return MStatus::kSuccess;
	}
	else if (plug == aimAxisAttr)
	{
		short aimAxis = handle.asShort();
		aimMatrix->aimAxis = (AimMatrix::Axis)aimAxis;

		MDataBlock dataBlock = forceCache();
		outputEnumValue(dataBlock, plug, aimAxis);
		
		return MStatus::kSuccess;
	}
	else if (plug == upAxisAttr)
	{
		short upAxis = handle.asShort();
		aimMatrix->upAxis = (AimMatrix::Axis)upAxis;

		MDataBlock dataBlock = forceCache();
		outputEnumValue(dataBlock, plug, upAxis);

		return MStatus::kSuccess;
	}

	return MPxTransform::validateAndSetValue(VALIDATE_AND_SET_VALUE_PARAMETERS);
}

/*	Description
	-----------
	During DG evaluation, compute is called after MPxTransformationMatrix::asMatrix() has already recalculated its internal matrix using the data from validateAndSetValue
	In this case, we are only responsible for ensuring the baseclass implementation is called so that any output attribute can be updated and cleaned
	When the Evaluation Graph is in use, it is the responsibility of compute() to retrieve the input data and cache it with the associated MPxTransformationMatrix
	In this case, the call to MPxTransformationMatrix::asMatrix() will occur after compute()    */
MStatus AimTransform::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	AimMatrix* aimMatrix = static_cast<AimMatrix*>(transformationMatrixPtr());

	if (plug == matrix || plug == inverseMatrix || 
		plug == worldMatrix || plug == worldInverseMatrix || 
		plug == parentMatrix || plug == parentInverseMatrix)
	{
		MDataHandle parentInverseFrameHandle = dataBlock.inputValue(parentInverseFrameAttr);
		aimMatrix->parentInverseFrame = parentInverseFrameHandle.asMatrix();

		MDataHandle rootPositionHandle = dataBlock.inputValue(rootPositionAttr);
		MMatrix rootFrame = rootPositionHandle.asMatrix();
		aimMatrix->rootPosition = MRS::extractTranslation(rootFrame, MSpace::kPostTransform);

		MDataHandle aimPositionHandle = dataBlock.inputValue(aimPositionAttr);
		MMatrix aimFrame = aimPositionHandle.asMatrix();
		aimMatrix->aimPosition = MRS::extractTranslation(aimFrame, MSpace::kPostTransform);

		MDataHandle upPositionHandle = dataBlock.inputValue(upPositionAttr);
		MMatrix upVectorFrame = upPositionHandle.asMatrix();
		aimMatrix->upPosition = MRS::extractTranslation(upVectorFrame, MSpace::kPostTransform);

		MDataHandle aimAxisHandle = dataBlock.inputValue(aimAxisAttr);
		aimMatrix->aimAxis = (AimMatrix::Axis)aimAxisHandle.asShort();

		MDataHandle upAxisHandle = dataBlock.inputValue(upAxisAttr);
		aimMatrix->upAxis = (AimMatrix::Axis)upAxisHandle.asShort();
	}

	return MPxTransform::compute(plug, dataBlock);
}