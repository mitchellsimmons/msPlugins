#include "packQuaternionArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackQuaternionArray::PackQuaternionArray() : MPxNode(), NodeHelper() {}
PackQuaternionArray::~PackQuaternionArray() {}

// ------ Attr ------
MObject PackQuaternionArray::inputQuaternionAttr;
MObject PackQuaternionArray::inputQuaternionXAttr;
MObject PackQuaternionArray::inputQuaternionYAttr;
MObject PackQuaternionArray::inputQuaternionZAttr;
MObject PackQuaternionArray::inputQuaternionWAttr;
MObject PackQuaternionArray::fillQuaternionAttr;
MObject PackQuaternionArray::fillQuaternionXAttr;
MObject PackQuaternionArray::fillQuaternionYAttr;
MObject PackQuaternionArray::fillQuaternionZAttr;
MObject PackQuaternionArray::fillQuaternionWAttr;
MObject PackQuaternionArray::fillLogicallyAttr;
MObject PackQuaternionArray::sizeAttr;
MObject PackQuaternionArray::outputQuaternionAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackQuaternionArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackQuaternionArray::initialize()
{
	MQuaternion in;
	std::vector<MQuaternion> out;

	createQuaternionAttribute(inputQuaternionAttr, inputQuaternionXAttr, inputQuaternionYAttr, inputQuaternionZAttr, inputQuaternionWAttr, 
		"input", "input", in, kArrayPreset | kKeyable);
	createQuaternionAttribute(fillQuaternionAttr, fillQuaternionXAttr, fillQuaternionYAttr, fillQuaternionZAttr, fillQuaternionWAttr, 
		"fill", "fill", in, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(outputQuaternionAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(inputQuaternionAttr);
	addAttribute(fillQuaternionAttr);
	addAttribute(fillLogicallyAttr);
	addAttribute(sizeAttr);
	addAttribute(outputQuaternionAttr);

	attributeAffects(inputQuaternionAttr, outputQuaternionAttr);
	attributeAffects(inputQuaternionXAttr, outputQuaternionAttr);
	attributeAffects(inputQuaternionYAttr, outputQuaternionAttr);
	attributeAffects(inputQuaternionZAttr, outputQuaternionAttr);
	attributeAffects(inputQuaternionWAttr, outputQuaternionAttr);
	attributeAffects(fillQuaternionAttr, outputQuaternionAttr);
	attributeAffects(fillQuaternionXAttr, outputQuaternionAttr);
	attributeAffects(fillQuaternionYAttr, outputQuaternionAttr);
	attributeAffects(fillQuaternionZAttr, outputQuaternionAttr);
	attributeAffects(fillQuaternionWAttr, outputQuaternionAttr);
	attributeAffects(fillLogicallyAttr, outputQuaternionAttr);
	attributeAffects(sizeAttr, outputQuaternionAttr);

	return MStatus::kSuccess;
}

MStatus PackQuaternionArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputQuaternionAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	MQuaternion fillQuaternion = inputQuaternionValue(dataBlock, fillQuaternionAttr, fillQuaternionXAttr, fillQuaternionYAttr, 
		fillQuaternionZAttr, fillQuaternionWAttr);
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_quaternions.resize(size);

	MArrayDataHandle inQuaternionArrayHandle = dataBlock.inputArrayValue(inputQuaternionAttr);

	unsigned int count = 0;
	unsigned int elementCount = std::min(inQuaternionArrayHandle.elementCount(), size);
	unsigned int elementIndex = 0;

	if (fillLogically)
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			if (inQuaternionArrayHandle.elementIndex() < size)
			{
				// Only record assigned indices as this value will be used to fill the end of the array if necessary
				elementIndex = inQuaternionArrayHandle.elementIndex();
				MDataHandle inQuaternionElementHandle = inQuaternionArrayHandle.inputValue();
				const double x = inQuaternionElementHandle.child(inputQuaternionXAttr).asDouble();
				const double y = inQuaternionElementHandle.child(inputQuaternionYAttr).asDouble();
				const double z = inQuaternionElementHandle.child(inputQuaternionZAttr).asDouble();
				const double w = inQuaternionElementHandle.child(inputQuaternionWAttr).asDouble();
				m_quaternions[elementIndex] = MQuaternion(x, y, z, w);

				while (count < elementIndex)
				{
					m_quaternions[count] = fillQuaternion;
					count++;
				}

				count++;
				inQuaternionArrayHandle.next();
			}
		}

		// If the size is greater than the elementCount, fill the array from the next index if elements exist, otherwise fill the entire array starting from index 0
		// We check the count has been incremented at least once, otherwise the array should be filled from the start
		elementIndex = elementCount > 0 && count > 0 ? elementIndex + 1 : 0;
		for (unsigned int i = elementIndex; i < size; i++)
			m_quaternions[i] = fillQuaternion;
	}
	else
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle inQuaternionElementHandle = inQuaternionArrayHandle.inputValue();
			const double x = inQuaternionElementHandle.child(inputQuaternionXAttr).asDouble();
			const double y = inQuaternionElementHandle.child(inputQuaternionYAttr).asDouble();
			const double z = inQuaternionElementHandle.child(inputQuaternionZAttr).asDouble();
			const double w = inQuaternionElementHandle.child(inputQuaternionWAttr).asDouble();
			m_quaternions[i] = MQuaternion(x, y, z, w);
			inQuaternionArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_quaternions[i] = fillQuaternion;
	}

	outputPluginDataArrayValue<QuaternionArrayData>(dataBlock, outputQuaternionAttr, m_quaternions);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------