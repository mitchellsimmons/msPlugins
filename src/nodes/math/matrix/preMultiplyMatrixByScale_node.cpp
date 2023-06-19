#include "preMultiplyMatrixByScale_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByScale::PreMultiplyMatrixByScale() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByScale::~PreMultiplyMatrixByScale() {}

// ------ Attr ------
MObject PreMultiplyMatrixByScale::inputAttr;
MObject PreMultiplyMatrixByScale::scaleAttr;
MObject PreMultiplyMatrixByScale::scaleXAttr;
MObject PreMultiplyMatrixByScale::scaleYAttr;
MObject PreMultiplyMatrixByScale::scaleZAttr;
MObject PreMultiplyMatrixByScale::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByScale::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByScale::initialize()
{
	MMatrix matrix;
	MVector scale = MVector::one;

	createVectorAttribute(scaleAttr, scaleXAttr, scaleYAttr, scaleZAttr, "scale", "scale", scale, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(scaleAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(scaleAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByScale::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MVector scale = inputVectorValue(dataBlock, scaleAttr);

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.preScale(&scale.x);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------