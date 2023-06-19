#include "power_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Power::Power() {}
Power::~Power() {}

// ------ Attr ------
MObject Power::baseAttr;
MObject Power::exponentAttr;
MObject Power::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Power::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Power::initialize()
{
	createDoubleAttribute(baseAttr, "base", "base", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(exponentAttr, "exponent", "exponent", 2.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(baseAttr);
	addAttribute(exponentAttr);
	addAttribute(outputAttr);

	attributeAffects(baseAttr, outputAttr);
	attributeAffects(exponentAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Power::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double base = inputDoubleValue(dataBlock, baseAttr);
	double exponent = inputDoubleValue(dataBlock, exponentAttr);
	
	outputDoubleValue(dataBlock, outputAttr, std::pow(base, exponent));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------