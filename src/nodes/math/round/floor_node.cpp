#include "floor_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Floor::Floor() {}
Floor::~Floor() {}

// ------ Attr ------
MObject Floor::inputAttr;
MObject Floor::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Floor::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Floor::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Floor::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, std::floor(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------