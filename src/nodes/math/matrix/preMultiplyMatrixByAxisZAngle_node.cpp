#include "preMultiplyMatrixByAxisZAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByAxisZAngle::PreMultiplyMatrixByAxisZAngle() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByAxisZAngle::~PreMultiplyMatrixByAxisZAngle() {}

// ------ Attr ------
MObject PreMultiplyMatrixByAxisZAngle::inputAttr;
MObject PreMultiplyMatrixByAxisZAngle::angleZAttr;
MObject PreMultiplyMatrixByAxisZAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByAxisZAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByAxisZAngle::initialize()
{
	MMatrix matrix;

	createAngleAttribute(angleZAttr, "angleZ", "angleZ", 0.0, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(angleZAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(angleZAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByAxisZAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	double angle = inputAngleValue(dataBlock, angleZAttr).asRadians();

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.preRotateInZ(angle);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------