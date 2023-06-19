#include "minElementAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MinElementAngle::MinElementAngle() {}
MinElementAngle::~MinElementAngle() {}

// ------ Attr ------
MObject MinElementAngle::inputAttr;
MObject MinElementAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MinElementAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MinElementAngle::initialize()
{
	std::vector<MAngle> inputs;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MinElementAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> inputs = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, inputAttr);

	auto min = std::min_element(inputs.cbegin(), inputs.cend(), [](const MAngle& lhs, const MAngle& rhs) -> bool { return lhs.asRadians() < rhs.asRadians(); });
	
	outputAngleValue(dataBlock, outputAttr, min != inputs.cend() ? *min : 0.0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------