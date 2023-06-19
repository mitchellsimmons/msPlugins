#include "clampInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ClampInt::ClampInt() {}
ClampInt::~ClampInt() {}

// ------ Attr ------
MObject ClampInt::inputAttr;
MObject ClampInt::minAttr;
MObject ClampInt::maxAttr;
MObject ClampInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ClampInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ClampInt::initialize()
{
	createIntAttribute(inputAttr, "input", "input", 0, kDefaultPreset | kKeyable);
	createIntAttribute(minAttr, "min", "min", 0, kDefaultPreset | kKeyable);
	createIntAttribute(maxAttr, "max", "max", 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(minAttr);
	addAttribute(maxAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(minAttr, outputAttr);
	attributeAffects(maxAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ClampInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input = inputIntValue(dataBlock, inputAttr);
	int min = inputIntValue(dataBlock, minAttr);
	int max = inputIntValue(dataBlock, maxAttr);

	outputIntValue(dataBlock, outputAttr, MRS::clamp(input, min, max));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------