#include "averageQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageQuaternion::AverageQuaternion() {}
AverageQuaternion::~AverageQuaternion() {}

// ------ Attr ------
MObject AverageQuaternion::inputAttr;
MObject AverageQuaternion::outputAttr;
MObject AverageQuaternion::outputXAttr;
MObject AverageQuaternion::outputYAttr;
MObject AverageQuaternion::outputZAttr;
MObject AverageQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageQuaternion::initialize()
{
	std::vector<MQuaternion> inputs;
	MQuaternion output;

	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AverageQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MQuaternion> inputs = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, inputAttr);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, MRS::averageQuaternion(inputs));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------