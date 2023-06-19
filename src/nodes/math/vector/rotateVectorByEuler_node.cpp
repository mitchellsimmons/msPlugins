#include "rotateVectorByEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RotateVectorByEuler::RotateVectorByEuler() {}
RotateVectorByEuler::~RotateVectorByEuler() {}

// ------ Attr ------
MObject RotateVectorByEuler::inputAttr;
MObject RotateVectorByEuler::inputXAttr;
MObject RotateVectorByEuler::inputYAttr;
MObject RotateVectorByEuler::inputZAttr;
MObject RotateVectorByEuler::rotationAttr;
MObject RotateVectorByEuler::rotationXAttr;
MObject RotateVectorByEuler::rotationYAttr;
MObject RotateVectorByEuler::rotationZAttr;
MObject RotateVectorByEuler::rotationOrderAttr;
MObject RotateVectorByEuler::outputAttr;
MObject RotateVectorByEuler::outputXAttr;
MObject RotateVectorByEuler::outputYAttr;
MObject RotateVectorByEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RotateVectorByEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RotateVectorByEuler::initialize()
{
	MVector vector = MVector::zero;
	std::unordered_map<const char*, short> rotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };


	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createEulerAttribute(rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr, "rotation", "rotation", vector, kDefaultPreset | kKeyable);
	createEnumAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder",rotationOrderFields, 0, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(rotationAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(rotationAttr, outputAttr);
	attributeAffects(rotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RotateVectorByEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input = inputVectorValue(dataBlock, inputAttr);
	MEulerRotation rotation = inputEulerValue(dataBlock, rotationAttr, rotationXAttr, rotationYAttr, rotationZAttr);
	short rotationOrder = inputEnumValue(dataBlock, rotationOrderAttr);
	rotation.order = (MEulerRotation::RotationOrder)rotationOrder;

	outputVectorValue(dataBlock, outputAttr, input.rotateBy(rotation));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------