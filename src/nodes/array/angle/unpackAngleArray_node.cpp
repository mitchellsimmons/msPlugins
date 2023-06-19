#include "unpackAngleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackAngleArray::UnpackAngleArray() : MPxNode(), NodeHelper() {}
UnpackAngleArray::~UnpackAngleArray() {}

// ------ Attr ------
MObject UnpackAngleArray::inputAttr;
MObject UnpackAngleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackAngleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackAngleArray::initialize()
{
	std::vector<MAngle> in;
	MAngle out;

	createPluginDataArrayAttribute<AngleArrayData>(inputAttr, "input", "input", in, kDefaultPreset);
	createAngleAttribute(outputAttr, "output", "output", out, kReadOnlyArrayPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus UnpackAngleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle inputDataArrayHandle = dataBlock.inputValue(inputAttr);
	MObject dataObj = inputDataArrayHandle.data();
	MFnPluginData fnData(dataObj);
	AngleArrayData* angleData = (AngleArrayData*)fnData.data();
	angleData->getArray(m_data);

	MArrayDataHandle outputArrayHandle = dataBlock.outputArrayValue(outputAttr);
	MArrayDataBuilder builder(&dataBlock, outputAttr, unsigned(m_data.size()));

	for (const MAngle& angle : m_data)
	{
		MDataHandle outputElementHandle = builder.addLast();
		outputElementHandle.setMAngle(angle);
	}

	outputArrayHandle.set(builder);
	outputArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------