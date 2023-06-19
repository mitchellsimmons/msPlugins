#include "unpackIntArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackIntArray::UnpackIntArray() : MPxNode(), NodeHelper() {}
UnpackIntArray::~UnpackIntArray() {}

// ------ Attr ------
MObject UnpackIntArray::inputAttr;
MObject UnpackIntArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackIntArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackIntArray::initialize()
{
	std::vector<int> in;

	createIntDataArrayAttribute(inputAttr, "input", "input", in, kDefaultPreset);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyArrayPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus UnpackIntArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle inputDataArrayHandle = dataBlock.inputValue(inputAttr);
	MArrayDataHandle outputArrayHandle = dataBlock.outputArrayValue(outputAttr);

	MObject dataObj = inputDataArrayHandle.data();
	MFnIntArrayData fnIntArray(dataObj);
	MArrayDataBuilder builder(&dataBlock, outputAttr, fnIntArray.length());

	for (unsigned int i = 0; i < fnIntArray.length(); i++)
	{
		MDataHandle outputElementHandle = builder.addElement(i);
		outputElementHandle.setInt(fnIntArray[i]);
	}

	outputArrayHandle.set(builder);
	outputArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------