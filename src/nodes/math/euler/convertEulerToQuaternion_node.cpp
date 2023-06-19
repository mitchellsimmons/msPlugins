#include "convertEulerToQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConvertEulerToQuaternion::ConvertEulerToQuaternion() {}
ConvertEulerToQuaternion::~ConvertEulerToQuaternion() {}

// ------ Attr ------
MObject ConvertEulerToQuaternion::rotationOrderAttr;
MObject ConvertEulerToQuaternion::inputAttr;
MObject ConvertEulerToQuaternion::inputXAttr;
MObject ConvertEulerToQuaternion::inputYAttr;
MObject ConvertEulerToQuaternion::inputZAttr;
MObject ConvertEulerToQuaternion::outputAttr;
MObject ConvertEulerToQuaternion::outputXAttr;
MObject ConvertEulerToQuaternion::outputYAttr;
MObject ConvertEulerToQuaternion::outputZAttr;
MObject ConvertEulerToQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ConvertEulerToQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ConvertEulerToQuaternion::initialize()
{
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MEulerRotation input;
	MQuaternion output;

	createEulerAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ConvertEulerToQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation input = inputEulerValue(dataBlock, inputAttr, inputXAttr, inputYAttr, inputZAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
	input.order = (MEulerRotation::RotationOrder)rotationOrder;

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, input.asQuaternion());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------