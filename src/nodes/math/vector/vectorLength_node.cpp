#include "vectorLength_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VectorLength::VectorLength() {}
VectorLength::~VectorLength() {}

// ------ Attr ------
MObject VectorLength::inputAttr;
MObject VectorLength::inputXAttr;
MObject VectorLength::inputYAttr;
MObject VectorLength::inputZAttr;
MObject VectorLength::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VectorLength::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VectorLength::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus VectorLength::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, v.length());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------