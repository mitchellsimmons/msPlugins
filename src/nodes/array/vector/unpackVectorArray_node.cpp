#include "unpackVectorArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackVectorArray::UnpackVectorArray() : MPxNode(), NodeHelper() {}
UnpackVectorArray::~UnpackVectorArray() {}

// ------ Attr ------
MObject UnpackVectorArray::inputVectorAttr;
MObject UnpackVectorArray::outputVectorAttr;
MObject UnpackVectorArray::outputVectorXAttr;
MObject UnpackVectorArray::outputVectorYAttr;
MObject UnpackVectorArray::outputVectorZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackVectorArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackVectorArray::initialize()
{
	std::vector<MVector> in;
	MVector out;

	createVectorDataArrayAttribute(inputVectorAttr, "input", "input", in, kDefaultPreset);
	createVectorAttribute(outputVectorAttr, outputVectorXAttr, outputVectorYAttr, outputVectorZAttr, "output", "output", out, kReadOnlyArrayPreset);

	addAttribute(inputVectorAttr);
	addAttribute(outputVectorAttr);

	attributeAffects(inputVectorAttr, outputVectorAttr);

	return MStatus::kSuccess;
}

MStatus UnpackVectorArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputVectorAttr &&
		plug.parent() != outputVectorAttr)
	{
		return MStatus::kUnknownParameter;
	}

	MDataHandle inVectorDataArrayHandle = dataBlock.inputValue(inputVectorAttr);
	MArrayDataHandle outVectorArrayHandle = dataBlock.outputArrayValue(outputVectorAttr);

	MObject vectorDataObj = inVectorDataArrayHandle.data();
	MFnVectorArrayData fnVector(vectorDataObj);
	MArrayDataBuilder builder(&dataBlock, outputVectorAttr, fnVector.length());

	for (unsigned int i = 0; i < fnVector.length(); i++)
	{
		MDataHandle outVectorElementHandle = builder.addElement(i);
		outVectorElementHandle.setMVector(fnVector[i]);
	}

	outVectorArrayHandle.set(builder);
	outVectorArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------