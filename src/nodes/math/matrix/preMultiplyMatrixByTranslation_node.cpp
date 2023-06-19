#include "preMultiplyMatrixByTranslation_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByTranslation::PreMultiplyMatrixByTranslation() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByTranslation::~PreMultiplyMatrixByTranslation() {}

// ------ Attr ------
MObject PreMultiplyMatrixByTranslation::inputAttr;
MObject PreMultiplyMatrixByTranslation::translationAttr;
MObject PreMultiplyMatrixByTranslation::translationXAttr;
MObject PreMultiplyMatrixByTranslation::translationYAttr;
MObject PreMultiplyMatrixByTranslation::translationZAttr;
MObject PreMultiplyMatrixByTranslation::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByTranslation::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByTranslation::initialize()
{
	MMatrix matrix;
	MVector translation = MVector::zero;

	createVectorAttribute(translationAttr, translationXAttr, translationYAttr, translationZAttr, "translation", "translation", translation, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(translationAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(translationAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByTranslation::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MVector translation = inputVectorValue(dataBlock, translationAttr);

	MRS::Matrix44<double> transformed(matrix.matrix);
	transformed = transformed.preTranslate(&translation.x);
	transformed.get(matrix);

	outputMatrixValue(dataBlock, outputAttr, matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------