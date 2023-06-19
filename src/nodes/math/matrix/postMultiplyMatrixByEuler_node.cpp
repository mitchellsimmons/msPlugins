#include "postMultiplyMatrixByEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByEuler::PostMultiplyMatrixByEuler() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByEuler::~PostMultiplyMatrixByEuler() {}

// ------ Attr ------
MObject PostMultiplyMatrixByEuler::inputAttr;
MObject PostMultiplyMatrixByEuler::rotationAttr;
MObject PostMultiplyMatrixByEuler::rotationXAttr;
MObject PostMultiplyMatrixByEuler::rotationYAttr;
MObject PostMultiplyMatrixByEuler::rotationZAttr;
MObject PostMultiplyMatrixByEuler::rotationOrderAttr;
MObject PostMultiplyMatrixByEuler::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByEuler::initialize()
{
	MMatrix matrix;
	MEulerRotation rotation;
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createEulerAttribute(rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr, "rotation", "rotation", rotation, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MEulerRotation rotation = inputEulerValue(dataBlock, rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
	rotation.order = (MEulerRotation::RotationOrder)rotationOrder;

	outputMatrixValue(dataBlock, outputAttr, matrix * rotation.asMatrix());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------