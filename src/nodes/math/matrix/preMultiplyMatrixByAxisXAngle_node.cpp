#include "preMultiplyMatrixByAxisXAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByAxisXAngle::PreMultiplyMatrixByAxisXAngle() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByAxisXAngle::~PreMultiplyMatrixByAxisXAngle() {}

// ------ Attr ------
MObject PreMultiplyMatrixByAxisXAngle::angleXAttr;
MObject PreMultiplyMatrixByAxisXAngle::inputAttr;
MObject PreMultiplyMatrixByAxisXAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByAxisXAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByAxisXAngle::initialize()
{
	MMatrix matrix;

	createAngleAttribute(angleXAttr, "angleX", "angleX", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(angleXAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(angleXAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByAxisXAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleXAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.preRotateInX(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------