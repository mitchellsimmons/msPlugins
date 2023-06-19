#include "postMultiplyMatrixByTranslation_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByTranslation::PostMultiplyMatrixByTranslation() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByTranslation::~PostMultiplyMatrixByTranslation() {}

// ------ Attr ------
MObject PostMultiplyMatrixByTranslation::inputAttr;
MObject PostMultiplyMatrixByTranslation::translationAttr;
MObject PostMultiplyMatrixByTranslation::translationXAttr;
MObject PostMultiplyMatrixByTranslation::translationYAttr;
MObject PostMultiplyMatrixByTranslation::translationZAttr;
MObject PostMultiplyMatrixByTranslation::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByTranslation::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByTranslation::initialize()
{
	MMatrix matrix;
	MVector translation = MVector::zero;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createVectorAttribute(translationAttr, translationXAttr, translationYAttr, translationZAttr, "translation", "translation", translation, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(translationAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(translationAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByTranslation::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MVector translation = inputVectorValue(dataBlock, translationAttr);

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.postTranslate(&translation.x);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------