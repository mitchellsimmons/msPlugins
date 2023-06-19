#include "convertEulerToMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConvertEulerToMatrix::ConvertEulerToMatrix() {}
ConvertEulerToMatrix::~ConvertEulerToMatrix() {}

// ------ Attr ------
MObject ConvertEulerToMatrix::inputAttr;
MObject ConvertEulerToMatrix::inputXAttr;
MObject ConvertEulerToMatrix::inputYAttr;
MObject ConvertEulerToMatrix::inputZAttr;
MObject ConvertEulerToMatrix::rotationOrderAttr;
MObject ConvertEulerToMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ConvertEulerToMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ConvertEulerToMatrix::initialize()
{
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MEulerRotation input;
	MMatrix output;

	createEulerAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ConvertEulerToMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation input = inputEulerValue(dataBlock, inputAttr, inputXAttr, inputYAttr, inputZAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
	input.order = (MEulerRotation::RotationOrder)rotationOrder;

	outputMatrixValue(dataBlock, outputAttr, input.asMatrix());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------