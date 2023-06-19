#include "unpackEulerArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackEulerArray::UnpackEulerArray() : MPxNode(), NodeHelper() {}
UnpackEulerArray::~UnpackEulerArray() {}

// ------ Attr ------
MObject UnpackEulerArray::inputRotateAttr;
MObject UnpackEulerArray::outputRotateAttr;
MObject UnpackEulerArray::outputRotateXAttr;
MObject UnpackEulerArray::outputRotateYAttr;
MObject UnpackEulerArray::outputRotateZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackEulerArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackEulerArray::initialize()
{
	std::vector<MEulerRotation> in;
	MEulerRotation out;

	createPluginDataArrayAttribute<EulerArrayData>(inputRotateAttr, "input", "input", in, kDefaultPreset);
	createEulerAttribute(outputRotateAttr, outputRotateXAttr, outputRotateYAttr, outputRotateZAttr, "output", "output", out, kReadOnlyArrayPreset);

	addAttribute(inputRotateAttr);
	addAttribute(outputRotateAttr);

	attributeAffects(inputRotateAttr, outputRotateAttr);

	return MStatus::kSuccess;
}

MStatus UnpackEulerArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputRotateAttr &&
		plug != outputRotateXAttr &&
		plug != outputRotateYAttr &&
		plug != outputRotateZAttr)
	{
		return MStatus::kUnknownParameter;
	}
	
	MDataHandle inRotateDataArrayHandle = dataBlock.inputValue(inputRotateAttr);
	MObject rotateDataObj = inRotateDataArrayHandle.data();
	MFnPluginData fnData(rotateDataObj);
	EulerArrayData* eulerData = (EulerArrayData*)fnData.data();
	eulerData->getArray(m_rotations);

	MArrayDataHandle outRotateArrayHandle = dataBlock.outputArrayValue(outputRotateAttr);
	MArrayDataBuilder builder(&dataBlock, outputRotateAttr, unsigned(m_rotations.size()));

	for (const MEulerRotation& rotation : m_rotations)
	{
		MDataHandle outRotateElementHandle = builder.addLast();
		MDataHandle outRotateXHandle = outRotateElementHandle.child(outputRotateXAttr);
		MDataHandle outRotateYHandle = outRotateElementHandle.child(outputRotateYAttr);
		MDataHandle outRotateZHandle = outRotateElementHandle.child(outputRotateZAttr);

		outRotateXHandle.setMAngle(MAngle(rotation.x));
		outRotateYHandle.setMAngle(MAngle(rotation.y));
		outRotateZHandle.setMAngle(MAngle(rotation.z));
	}

	outRotateArrayHandle.set(builder);
	outRotateArrayHandle.setAllClean();
	
	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------