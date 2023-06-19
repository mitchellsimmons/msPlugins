#include "maxElementAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MaxElementAngle::MaxElementAngle() {}
MaxElementAngle::~MaxElementAngle() {}

// ------ Attr ------
MObject MaxElementAngle::inputAttr;
MObject MaxElementAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MaxElementAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MaxElementAngle::initialize()
{
	std::vector<MAngle> inputs;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MaxElementAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> inputs = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, inputAttr);

	auto max = std::max_element(inputs.cbegin(), inputs.cend(), [](const MAngle& lhs, const MAngle& rhs) -> bool { return lhs.asRadians() < rhs.asRadians(); });
	
	outputAngleValue(dataBlock, outputAttr, max != inputs.cend() ? *max : 0.0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------