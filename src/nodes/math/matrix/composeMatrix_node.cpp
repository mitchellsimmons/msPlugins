#include "composeMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ComposeMatrix::ComposeMatrix() : MPxNode(), NodeHelper() {}
ComposeMatrix::~ComposeMatrix() {}

// ------ Attr ------
MObject ComposeMatrix::translationAttr;
MObject ComposeMatrix::translationXAttr;
MObject ComposeMatrix::translationYAttr;
MObject ComposeMatrix::translationZAttr;
MObject ComposeMatrix::rotationAttr;
MObject ComposeMatrix::rotationXAttr;
MObject ComposeMatrix::rotationYAttr;
MObject ComposeMatrix::rotationZAttr;
MObject ComposeMatrix::scaleAttr;
MObject ComposeMatrix::scaleXAttr;
MObject ComposeMatrix::scaleYAttr;
MObject ComposeMatrix::scaleZAttr;
MObject ComposeMatrix::quaternionAttr;
MObject ComposeMatrix::quaternionXAttr;
MObject ComposeMatrix::quaternionYAttr;
MObject ComposeMatrix::quaternionZAttr;
MObject ComposeMatrix::quaternionWAttr;
MObject ComposeMatrix::rotationOrderAttr;
MObject ComposeMatrix::useEulerRotationAttr;
MObject ComposeMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ComposeMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ComposeMatrix::initialize()
{
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MVector translation;
	MEulerRotation rotation;
	MVector scale = MVector::one;
	MQuaternion quaternion;
	MMatrix output;

	createVectorAttribute(translationAttr, translationXAttr, translationYAttr, translationZAttr,
		"translation", "translation", translation, kDefaultPreset | kKeyable);
	createEulerAttribute(rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr,
		"rotation", "rotation", rotation, kDefaultPreset | kKeyable);
	createVectorAttribute(scaleAttr, scaleXAttr, scaleYAttr, scaleZAttr,
		"scale", "scale", scale, kDefaultPreset | kKeyable);
	createQuaternionAttribute(quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr,
		"quaternion", "quaternion", quaternion, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createBoolAttribute(useEulerRotationAttr, "useEulerRotation", "useEulerRotation", true, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(translationAttr);
	addAttribute(rotationAttr);
	addAttribute(scaleAttr);
	addAttribute(quaternionAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(useEulerRotationAttr);
	addAttribute(outputAttr);

	attributeAffects(translationAttr, outputAttr);
	attributeAffects(rotationAttr, outputAttr);
	attributeAffects(scaleAttr, outputAttr);
	attributeAffects(quaternionAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);
	attributeAffects(useEulerRotationAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ComposeMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector translation = inputVectorValue(dataBlock, translationAttr);
	MVector scale = inputVectorValue(dataBlock, scaleAttr);
	bool useEuler = inputBoolValue(dataBlock, useEulerRotationAttr);

	if (useEuler)
	{
		MEulerRotation rotation = inputEulerValue(dataBlock, rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr);
		short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
		rotation.order = (MEulerRotation::RotationOrder)rotationOrder;
		outputMatrixValue(dataBlock, outputAttr, MRS::composeMatrix(translation, rotation, scale));
	}
	else
	{
		MQuaternion rotation = inputQuaternionValue(dataBlock, quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr);
		outputMatrixValue(dataBlock, outputAttr, MRS::composeMatrix(translation, rotation, scale));
	}

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------