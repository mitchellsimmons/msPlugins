#include "packIntArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackIntArray::PackIntArray() : MPxNode(), NodeHelper() {}
PackIntArray::~PackIntArray() {}

// ------ Attr ------
MObject PackIntArray::inputAttr;
MObject PackIntArray::fillAttr;
MObject PackIntArray::fillLogicallyAttr;
MObject PackIntArray::sizeAttr;
MObject PackIntArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackIntArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackIntArray::initialize()
{
	std::vector<int> out;

	createIntAttribute(inputAttr, "input", "input", 0, kArrayPreset | kKeyable);
	createIntAttribute(fillAttr, "fill", "fill", 0, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createIntDataArrayAttribute(outputAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(fillAttr);
	addAttribute(fillLogicallyAttr);
	addAttribute(sizeAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(fillAttr, outputAttr);
	attributeAffects(fillLogicallyAttr, outputAttr);
	attributeAffects(sizeAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PackIntArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	int fill = dataBlock.inputValue(fillAttr).asInt();
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_data.setLength(size);

	MArrayDataHandle inputArrayHandle = dataBlock.inputArrayValue(inputAttr);
	MDataHandle outputDataArrayHandle = dataBlock.outputValue(outputAttr);

	unsigned int count = 0;
	unsigned int elementCount = std::min(inputArrayHandle.elementCount(), size);
	unsigned int elementIndex = 0;

	if (fillLogically)
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			if (inputArrayHandle.elementIndex() < size)
			{
				// Only record assigned indices as this value will be used to fill the end of the array if necessary
				elementIndex = inputArrayHandle.elementIndex();
				MDataHandle inputElementHandle = inputArrayHandle.inputValue();
				m_data[elementIndex] = inputElementHandle.asInt();

				while (count < elementIndex)
				{
					m_data[count] = fill;
					count++;
				}

				count++;
				inputArrayHandle.next();
			}
		}

		// If the size is greater than the elementCount, fill the array from the next index if elements exist, otherwise fill the entire array starting from index 0
		// We check the count has been incremented at least once, otherwise the array should be filled from the start
		elementIndex = elementCount > 0 && count > 0 ? elementIndex + 1 : 0;
		for (unsigned int i = elementIndex; i < size; i++)
			m_data[i] = fill;
	}
	else
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle inputElementHandle = inputArrayHandle.inputValue();
			m_data[i] = inputElementHandle.asInt();
			inputArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_data[i] = fill;
	}

	MFnIntArrayData fnArray;
	MObject outputObj = fnArray.create(m_data);
	outputDataArrayHandle.setMObject(outputObj);
	outputDataArrayHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------