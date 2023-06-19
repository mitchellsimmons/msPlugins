#include "preMultiplyMatrixByAxisYAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByAxisYAngle::PreMultiplyMatrixByAxisYAngle() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByAxisYAngle::~PreMultiplyMatrixByAxisYAngle() {}

// ------ Attr ------
MObject PreMultiplyMatrixByAxisYAngle::inputAttr;
MObject PreMultiplyMatrixByAxisYAngle::angleYAttr;
MObject PreMultiplyMatrixByAxisYAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByAxisYAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByAxisYAngle::initialize()
{
	MMatrix matrix;

	createAngleAttribute(angleYAttr, "angleY", "angleY", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(angleYAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(angleYAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByAxisYAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleYAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.preRotateInY(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------