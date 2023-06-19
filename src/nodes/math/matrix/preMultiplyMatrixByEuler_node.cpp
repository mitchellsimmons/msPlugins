#include "preMultiplyMatrixByEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByEuler::PreMultiplyMatrixByEuler() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByEuler::~PreMultiplyMatrixByEuler() {}

// ------ Attr ------
MObject PreMultiplyMatrixByEuler::rotationAttr;
MObject PreMultiplyMatrixByEuler::rotationXAttr;
MObject PreMultiplyMatrixByEuler::rotationYAttr;
MObject PreMultiplyMatrixByEuler::rotationZAttr;
MObject PreMultiplyMatrixByEuler::inputAttr;
MObject PreMultiplyMatrixByEuler::rotationOrderAttr;
MObject PreMultiplyMatrixByEuler::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByEuler::initialize()
{
	MMatrix matrix;
	MEulerRotation rotation;
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };

	createEulerAttribute(rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr, "rotation", "rotation", rotation, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(rotationAttr);
	addAttribute(inputAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(rotationAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MEulerRotation rotation = inputEulerValue(dataBlock, rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
	rotation.order = (MEulerRotation::RotationOrder)rotationOrder;

	outputMatrixValue(dataBlock, outputAttr, rotation.asMatrix() * matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------