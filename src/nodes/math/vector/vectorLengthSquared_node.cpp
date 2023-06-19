#include "vectorLengthSquared_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VectorLengthSquared::VectorLengthSquared() {}
VectorLengthSquared::~VectorLengthSquared() {}

// ------ Attr ------
MObject VectorLengthSquared::inputAttr;
MObject VectorLengthSquared::inputXAttr;
MObject VectorLengthSquared::inputYAttr;
MObject VectorLengthSquared::inputZAttr;
MObject VectorLengthSquared::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VectorLengthSquared::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VectorLengthSquared::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus VectorLengthSquared::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, v.x * v.x + v.y * v.y + v.z * v.z);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------