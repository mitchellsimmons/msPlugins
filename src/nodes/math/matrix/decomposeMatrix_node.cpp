#include "decomposeMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DecomposeMatrix::DecomposeMatrix() : MPxNode(), NodeHelper() {}
DecomposeMatrix::~DecomposeMatrix() {}

// ------ Attr ------
MObject DecomposeMatrix::inputAttr;
MObject DecomposeMatrix::rotationOrderAttr;
MObject DecomposeMatrix::outputTranslationAttr;
MObject DecomposeMatrix::outputTranslationXAttr;
MObject DecomposeMatrix::outputTranslationYAttr;
MObject DecomposeMatrix::outputTranslationZAttr;
MObject DecomposeMatrix::outputRotationAttr;
MObject DecomposeMatrix::outputRotationXAttr;
MObject DecomposeMatrix::outputRotationYAttr;
MObject DecomposeMatrix::outputRotationZAttr;
MObject DecomposeMatrix::outputScaleAttr;
MObject DecomposeMatrix::outputScaleXAttr;
MObject DecomposeMatrix::outputScaleYAttr;
MObject DecomposeMatrix::outputScaleZAttr;
MObject DecomposeMatrix::outputQuaternionAttr;
MObject DecomposeMatrix::outputQuaternionXAttr;
MObject DecomposeMatrix::outputQuaternionYAttr;
MObject DecomposeMatrix::outputQuaternionZAttr;
MObject DecomposeMatrix::outputQuaternionWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DecomposeMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DecomposeMatrix::initialize()
{
	MMatrix input;
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MVector outputTranslation;
	MEulerRotation outputRotation;
	MVector outputScale = MVector::one;
	MQuaternion outputQuaternion;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createVectorAttribute(outputTranslationAttr, outputTranslationXAttr, outputTranslationYAttr, outputTranslationZAttr,
		"outputTranslation", "outputTranslation", outputTranslation, kReadOnlyPreset);
	createEulerAttribute(outputRotationAttr, outputRotationXAttr, outputRotationYAttr, outputRotationZAttr,
		"outputRotation", "outputRotation", outputRotation, kReadOnlyPreset);
	createVectorAttribute(outputScaleAttr, outputScaleXAttr, outputScaleYAttr, outputScaleZAttr,
		"outputScale", "outputScale", outputScale, kReadOnlyPreset);
	createQuaternionAttribute(outputQuaternionAttr, outputQuaternionXAttr, outputQuaternionYAttr, outputQuaternionZAttr, outputQuaternionWAttr,
		"outputQuaternion", "outputQuaternion", outputQuaternion, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputTranslationAttr);
	addAttribute(outputRotationAttr);
	addAttribute(outputScaleAttr);
	addAttribute(outputQuaternionAttr);

	attributeAffects(inputAttr, outputTranslationAttr);
	attributeAffects(inputAttr, outputRotationAttr);
	attributeAffects(rotationOrderAttr, outputRotationAttr);
	attributeAffects(inputAttr, outputScaleAttr);
	attributeAffects(inputAttr, outputQuaternionAttr);

	return MStatus::kSuccess;
}

MStatus DecomposeMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug == outputTranslationXAttr || plug == outputTranslationYAttr || plug == outputTranslationZAttr || plug == outputTranslationAttr)
	{
		MMatrix input = inputMatrixValue(dataBlock, inputAttr);
		outputVectorValue(dataBlock, outputTranslationAttr, MRS::extractTranslation(input));
	}
	else if (plug == outputRotationXAttr || plug == outputRotationYAttr || plug == outputRotationZAttr || plug == outputRotationAttr)
	{
		MMatrix input = inputMatrixValue(dataBlock, inputAttr);
		short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
		outputEulerValue(dataBlock, outputRotationAttr, outputRotationXAttr, outputRotationYAttr, outputRotationZAttr, 
			MRS::extractEulerRotation(input, (MEulerRotation::RotationOrder)rotationOrder));
	}
	else if (plug == outputScaleXAttr || plug == outputScaleYAttr || plug == outputScaleZAttr || plug == outputScaleAttr)
	{
		MMatrix input = inputMatrixValue(dataBlock, inputAttr);
		outputVectorValue(dataBlock, outputScaleAttr, MRS::extractScale(input));
	}
	else if (plug == outputQuaternionXAttr || plug == outputQuaternionYAttr || plug == outputQuaternionZAttr || plug == outputQuaternionWAttr || plug == outputQuaternionAttr)
	{
		MMatrix input = inputMatrixValue(dataBlock, inputAttr);
		outputQuaternionValue(dataBlock, outputQuaternionAttr, outputQuaternionXAttr, outputQuaternionYAttr, outputQuaternionZAttr, outputQuaternionWAttr, 
			MRS::extractQuaternionRotation(input));
	}
	else
		return MStatus::kUnknownParameter;

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------