#include "postMultiplyMatrixByAxisXAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByAxisXAngle::PostMultiplyMatrixByAxisXAngle() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByAxisXAngle::~PostMultiplyMatrixByAxisXAngle() {}

// ------ Attr ------
MObject PostMultiplyMatrixByAxisXAngle::inputAttr;
MObject PostMultiplyMatrixByAxisXAngle::angleXAttr;
MObject PostMultiplyMatrixByAxisXAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByAxisXAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByAxisXAngle::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createAngleAttribute(angleXAttr, "angleX", "angleX", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(angleXAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(angleXAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByAxisXAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleXAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.postRotateInX(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------