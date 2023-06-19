#include "sumAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SumAngle::SumAngle() {}
SumAngle::~SumAngle() {}

// ------ Attr ------
MObject SumAngle::inputAttr;
MObject SumAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SumAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SumAngle::initialize()
{
	std::vector<MAngle> inputs;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SumAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> inputs = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, inputAttr);

	double sum = 0.0;
	for (const MAngle& input : inputs)
		sum += input.asRadians();

	outputAngleValue(dataBlock, outputAttr, sum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------