#include "extractEulerFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractEulerFromMatrix::ExtractEulerFromMatrix() : MPxNode(), NodeHelper() {}
ExtractEulerFromMatrix::~ExtractEulerFromMatrix() {}

// ------ Attr ------
MObject ExtractEulerFromMatrix::inputAttr;
MObject ExtractEulerFromMatrix::rotationOrderAttr;
MObject ExtractEulerFromMatrix::outputAttr;
MObject ExtractEulerFromMatrix::outputXAttr;
MObject ExtractEulerFromMatrix::outputYAttr;
MObject ExtractEulerFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractEulerFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractEulerFromMatrix::initialize()
{
	MMatrix input;
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	MEulerRotation output;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createEulerAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractEulerFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, MRS::extractEulerRotation(input, (MEulerRotation::RotationOrder)rotationOrder));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------