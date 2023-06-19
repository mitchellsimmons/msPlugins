#include "aim_node.h"

/*	Description
	-----------
	This node outputs a matrix which encompasses the basic functionality of an aim constraint.
	It is designed for cases where a non-hierarchical representation of the transform is required.
	When a DAG representation is required, the `aimTransform` node can be used as substitute.
	Input axes and positions are used to define an output frame, in the same space as the input frames.

	Attributes
	----------
	rootPosition - matrix
		Specifies the input space position of the output transform.

	aimPosition - matrix
		Specifies the input space position that the output transform's `aimAxis` should aim towards.

	upPosition - matrix
		Specifies the input space position that the output transform's `upAxis` should attempt to aim towards.

	aimAxis - enum
		Specifies the axis that the output transform should use to aim at the `aimPosition`.

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
		Specifies the axis that the output transform should use to aim at the `upPosition`.
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

	output - matrix
		The resulting input space transform.
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Aim::Aim() {}
Aim::~Aim() {}

// ------ Attr ------
MObject Aim::rootPositionAttr;
MObject Aim::aimPositionAttr;
MObject Aim::upPositionAttr;
MObject Aim::aimAxisAttr;
MObject Aim::upAxisAttr;
MObject Aim::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Aim::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Aim::initialize()
{
	std::unordered_map<const char*, short> fields{ {"+x", 0}, {"+y", 1}, {"+z", 2}, {"-x", 3}, {"-y", 4}, {"-z", 5} };
	createMatrixAttribute(rootPositionAttr, "rootPosition", "rootPosition", MMatrix::identity, kDefaultPreset);
	createMatrixAttribute(aimPositionAttr, "aimPosition", "aimPosition", MMatrix::identity, kDefaultPreset);
	createMatrixAttribute(upPositionAttr, "upPosition", "upPosition", MMatrix::identity, kDefaultPreset);
	createEnumAttribute(aimAxisAttr, "aimAxis", "aimAxis", fields, 0, kDefaultPreset | kKeyable);
	createEnumAttribute(upAxisAttr, "upAxis", "upAxis", fields, 1, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", MMatrix::identity, kReadOnlyPreset);

	addAttribute(rootPositionAttr);
	addAttribute(aimPositionAttr);
	addAttribute(upPositionAttr);
	addAttribute(aimAxisAttr);
	addAttribute(upAxisAttr);
	addAttribute(outputAttr);

	attributeAffects(rootPositionAttr, outputAttr);
	attributeAffects(aimPositionAttr, outputAttr);
	attributeAffects(upPositionAttr, outputAttr);
	attributeAffects(aimAxisAttr, outputAttr);
	attributeAffects(upAxisAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Aim::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	// Inputs
	MMatrix rootFrame = inputMatrixValue(dataBlock, rootPositionAttr);
	MVector rootPosition = MRS::extractTranslation(rootFrame);
	MMatrix aimFrame = inputMatrixValue(dataBlock, aimPositionAttr);
	MVector aimPosition = MRS::extractTranslation(aimFrame);
	MMatrix upFrame = inputMatrixValue(dataBlock, upPositionAttr);
	MVector upPosition = MRS::extractTranslation(upFrame);
	Axis aimAxis = (Axis)inputEnumValue(dataBlock, aimAxisAttr);
	Axis upAxis = (Axis)inputEnumValue(dataBlock, upAxisAttr);

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

	outputMatrixValue(dataBlock, outputAttr, outputFrame);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------