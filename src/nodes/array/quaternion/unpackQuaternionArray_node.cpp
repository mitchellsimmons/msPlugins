#include "unpackQuaternionArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackQuaternionArray::UnpackQuaternionArray() : MPxNode(), NodeHelper() {}
UnpackQuaternionArray::~UnpackQuaternionArray() {}

// ------ Attr ------
MObject UnpackQuaternionArray::inputQuaternionAttr;
MObject UnpackQuaternionArray::outputQuaternionAttr;
MObject UnpackQuaternionArray::outputQuaternionXAttr;
MObject UnpackQuaternionArray::outputQuaternionYAttr;
MObject UnpackQuaternionArray::outputQuaternionZAttr;
MObject UnpackQuaternionArray::outputQuaternionWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackQuaternionArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackQuaternionArray::initialize()
{
	std::vector<MQuaternion> in;
	MQuaternion out;

	createPluginDataArrayAttribute<QuaternionArrayData>(inputQuaternionAttr, "input", "input", in, kDefaultPreset);
	createQuaternionAttribute(outputQuaternionAttr, outputQuaternionXAttr, outputQuaternionYAttr, outputQuaternionZAttr, outputQuaternionWAttr, 
		"output", "output", out, kReadOnlyArrayPreset);

	addAttribute(inputQuaternionAttr);
	addAttribute(outputQuaternionAttr);

	attributeAffects(inputQuaternionAttr, outputQuaternionAttr);

	return MStatus::kSuccess;
}

MStatus UnpackQuaternionArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputQuaternionAttr &&
		plug != outputQuaternionXAttr &&
		plug != outputQuaternionYAttr &&
		plug != outputQuaternionZAttr &&
		plug != outputQuaternionWAttr)
	{
		return MStatus::kUnknownParameter;
	}

	MDataHandle inQuaternionDataArrayHandle = dataBlock.inputValue(inputQuaternionAttr);
	MObject quaternionDataObj = inQuaternionDataArrayHandle.data();
	MFnPluginData fnData(quaternionDataObj);
	QuaternionArrayData* quaternionData = (QuaternionArrayData*)fnData.data();
	quaternionData->getArray(m_quaternions);

	MArrayDataHandle outQuaternionArrayHandle = dataBlock.outputArrayValue(outputQuaternionAttr);
	MArrayDataBuilder builder(&dataBlock, outputQuaternionAttr, unsigned(m_quaternions.size()));

	for (const MQuaternion& quaternion : m_quaternions)
	{
		MDataHandle outQuaternionlementHandle = builder.addLast();
		MDataHandle outQuaternionXHandle = outQuaternionlementHandle.child(outputQuaternionXAttr);
		MDataHandle outQuaternionYHandle = outQuaternionlementHandle.child(outputQuaternionYAttr);
		MDataHandle outQuaternionZHandle = outQuaternionlementHandle.child(outputQuaternionZAttr);
		MDataHandle outQuaternionWHandle = outQuaternionlementHandle.child(outputQuaternionWAttr);

		outQuaternionXHandle.setDouble(quaternion.x);
		outQuaternionYHandle.setDouble(quaternion.y);
		outQuaternionZHandle.setDouble(quaternion.z);
		outQuaternionWHandle.setDouble(quaternion.w);
	}

	outQuaternionArrayHandle.set(builder);
	outQuaternionArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------