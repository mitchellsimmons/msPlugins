#include "postMultiplyMatrixByAxisYAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByAxisYAngle::PostMultiplyMatrixByAxisYAngle() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByAxisYAngle::~PostMultiplyMatrixByAxisYAngle() {}

// ------ Attr ------
MObject PostMultiplyMatrixByAxisYAngle::inputAttr;
MObject PostMultiplyMatrixByAxisYAngle::angleYAttr;
MObject PostMultiplyMatrixByAxisYAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByAxisYAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByAxisYAngle::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createAngleAttribute(angleYAttr, "angleY", "angleY", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(angleYAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(angleYAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByAxisYAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleYAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.postRotateInY(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------