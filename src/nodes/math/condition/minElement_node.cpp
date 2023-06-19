#include "minElement_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MinElement::MinElement() {}
MinElement::~MinElement() {}

// ------ Attr ------
MObject MinElement::inputAttr;
MObject MinElement::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MinElement::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MinElement::initialize()
{
	std::vector<double> inputs;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MinElement::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> inputs = inputDoubleDataArrayValue(dataBlock, inputAttr);

	auto min = std::min_element(inputs.cbegin(), inputs.cend());

	outputDoubleValue(dataBlock, outputAttr, min != inputs.cend() ? *min : 0.0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------