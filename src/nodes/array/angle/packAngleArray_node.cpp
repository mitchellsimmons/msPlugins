#include "packAngleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackAngleArray::PackAngleArray() : MPxNode(), NodeHelper() {}
PackAngleArray::~PackAngleArray() {}

// ------ Attr ------
MObject PackAngleArray::inputAttr;
MObject PackAngleArray::fillAttr;
MObject PackAngleArray::fillLogicallyAttr;
MObject PackAngleArray::sizeAttr;
MObject PackAngleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackAngleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackAngleArray::initialize()
{
	MAngle in;
	std::vector<MAngle> out;

	createAngleAttribute(inputAttr,"input", "input", in, kArrayPreset | kKeyable);
	createAngleAttribute(fillAttr, "fill", "fill", in, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<AngleArrayData, MAngle>(outputAttr, "output", "output", out, kReadOnlyPreset);

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

MStatus PackAngleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	MAngle fill = inputAngleValue(dataBlock, fillAttr);
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_data.resize(size);

	MArrayDataHandle inputArrayHandle = dataBlock.inputArrayValue(inputAttr);

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
				m_data[elementIndex] = inputElementHandle.asAngle();

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
			m_data[i] = inputElementHandle.asAngle();
			inputArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_data[i] = fill;
	}

	outputPluginDataArrayValue<AngleArrayData>(dataBlock, outputAttr, m_data);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------