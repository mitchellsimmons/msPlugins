#include "multiplyQuaternionArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyQuaternionArray::MultiplyQuaternionArray() {}
MultiplyQuaternionArray::~MultiplyQuaternionArray() {}

// ------ Attr ------
MObject MultiplyQuaternionArray::inputAttr;
MObject MultiplyQuaternionArray::outputAttr;
MObject MultiplyQuaternionArray::outputXAttr;
MObject MultiplyQuaternionArray::outputYAttr;
MObject MultiplyQuaternionArray::outputZAttr;
MObject MultiplyQuaternionArray::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyQuaternionArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyQuaternionArray::initialize()
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

MStatus MultiplyQuaternionArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MQuaternion> inputs = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, inputAttr);
	size_t count = inputs.size();

	MQuaternion qAverage = MQuaternion::identity;
	if (count)
	{
		qAverage = inputs[0];
		for (unsigned int i = 1; i < count; ++i)
			qAverage *= inputs[i];
	}

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, qAverage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------