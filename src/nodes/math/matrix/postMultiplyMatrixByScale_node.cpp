#include "postMultiplyMatrixByScale_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByScale::PostMultiplyMatrixByScale() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByScale::~PostMultiplyMatrixByScale() {}

// ------ Attr ------
MObject PostMultiplyMatrixByScale::inputAttr;
MObject PostMultiplyMatrixByScale::scaleAttr;
MObject PostMultiplyMatrixByScale::scaleXAttr;
MObject PostMultiplyMatrixByScale::scaleYAttr;
MObject PostMultiplyMatrixByScale::scaleZAttr;
MObject PostMultiplyMatrixByScale::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByScale::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByScale::initialize()
{
	MMatrix matrix;
	MVector scale = MVector::one;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createVectorAttribute(scaleAttr, scaleXAttr, scaleYAttr, scaleZAttr, "scale", "scale", scale, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(scaleAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(scaleAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByScale::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MVector scale = inputVectorValue(dataBlock, scaleAttr);

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.postScale(&scale.x);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------