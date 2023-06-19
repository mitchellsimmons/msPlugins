#include "convertQuaternionToEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConvertQuaternionToEuler::ConvertQuaternionToEuler() {}
ConvertQuaternionToEuler::~ConvertQuaternionToEuler() {}

// ------ Attr ------
MObject ConvertQuaternionToEuler::inputAttr;
MObject ConvertQuaternionToEuler::inputXAttr;
MObject ConvertQuaternionToEuler::inputYAttr;
MObject ConvertQuaternionToEuler::inputZAttr;
MObject ConvertQuaternionToEuler::inputWAttr;
MObject ConvertQuaternionToEuler::rotationOrderAttr;
MObject ConvertQuaternionToEuler::outputAttr;
MObject ConvertQuaternionToEuler::outputXAttr;
MObject ConvertQuaternionToEuler::outputYAttr;
MObject ConvertQuaternionToEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ConvertQuaternionToEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ConvertQuaternionToEuler::initialize()
{
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MQuaternion input = MQuaternion::identity;
	MEulerRotation output = MEulerRotation::identity;

	createQuaternionAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, inputWAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createEulerAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ConvertQuaternionToEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MQuaternion input = inputQuaternionValue(dataBlock, inputAttr, inputXAttr, inputYAttr, inputZAttr, inputWAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);

	MEulerRotation output = input.asEulerRotation();
	output.reorderIt((MEulerRotation::RotationOrder)rotationOrder);

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------