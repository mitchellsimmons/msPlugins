#include "postMultiplyMatrixByAxisZAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByAxisZAngle::PostMultiplyMatrixByAxisZAngle() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByAxisZAngle::~PostMultiplyMatrixByAxisZAngle() {}

// ------ Attr ------
MObject PostMultiplyMatrixByAxisZAngle::inputAttr;
MObject PostMultiplyMatrixByAxisZAngle::angleZAttr;
MObject PostMultiplyMatrixByAxisZAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByAxisZAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByAxisZAngle::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createAngleAttribute(angleZAttr, "angleZ", "angleZ", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(angleZAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(angleZAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByAxisZAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleZAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.postRotateInZ(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------