#include "maxElement_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MaxElement::MaxElement() {}
MaxElement::~MaxElement() {}

// ------ Attr ------
MObject MaxElement::inputAttr;
MObject MaxElement::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MaxElement::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MaxElement::initialize()
{
	std::vector<double> inputs;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MaxElement::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> inputs = inputDoubleDataArrayValue(dataBlock, inputAttr);

	auto max = std::max_element(inputs.cbegin(), inputs.cend());

	outputDoubleValue(dataBlock, outputAttr, max != inputs.cend() ? *max : 0.0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------