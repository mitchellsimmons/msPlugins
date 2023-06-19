#include "unpackDoubleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackDoubleArray::UnpackDoubleArray() : MPxNode(), NodeHelper() {}
UnpackDoubleArray::~UnpackDoubleArray() {}

// ------ Attr ------
MObject UnpackDoubleArray::inputAttr;
MObject UnpackDoubleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackDoubleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackDoubleArray::initialize()
{
	std::vector<double> in;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", in, kDefaultPreset);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyArrayPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus UnpackDoubleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle inputDataArrayHandle = dataBlock.inputValue(inputAttr);
	MArrayDataHandle outputArrayHandle = dataBlock.outputArrayValue(outputAttr);

	MObject dataObj = inputDataArrayHandle.data();
	MFnDoubleArrayData fnDoubleArray(dataObj);
	MArrayDataBuilder builder(&dataBlock, outputAttr, fnDoubleArray.length());

	for (unsigned int i = 0; i < fnDoubleArray.length(); i++)
	{
		MDataHandle outputElementHandle = builder.addElement(i);
		outputElementHandle.setDouble(fnDoubleArray[i]);
	}

	outputArrayHandle.set(builder);
	outputArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------