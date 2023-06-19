#include "averageAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageAngle::AverageAngle() {}
AverageAngle::~AverageAngle() {}

// ------ Attr ------
MObject AverageAngle::inputAttr;
MObject AverageAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageAngle::initialize()
{
	std::vector<MAngle> inputs;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AverageAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> inputs = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, inputAttr);

	size_t count = inputs.size();
	double sum = 0.0;
	for (const MAngle& angle : inputs)
		sum += angle.asRadians();

	outputAngleValue(dataBlock, outputAttr, count == 0 ? 0.0 : sum / count);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------