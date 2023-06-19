#include "packEulerArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackEulerArray::PackEulerArray() : MPxNode(), NodeHelper() {}
PackEulerArray::~PackEulerArray() {}

// ------ Attr ------
MObject PackEulerArray::inputRotateAttr;
MObject PackEulerArray::inputRotateXAttr;
MObject PackEulerArray::inputRotateYAttr;
MObject PackEulerArray::inputRotateZAttr;
MObject PackEulerArray::fillRotateAttr;
MObject PackEulerArray::fillRotateXAttr;
MObject PackEulerArray::fillRotateYAttr;
MObject PackEulerArray::fillRotateZAttr;
MObject PackEulerArray::fillLogicallyAttr;
MObject PackEulerArray::sizeAttr;
MObject PackEulerArray::outputRotateAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackEulerArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackEulerArray::initialize()
{
	MEulerRotation in;
	std::vector<MEulerRotation> out;

	createEulerAttribute(inputRotateAttr, inputRotateXAttr, inputRotateYAttr, inputRotateZAttr, "input", "input", in, kArrayPreset | kKeyable);
	createEulerAttribute(fillRotateAttr, fillRotateXAttr, fillRotateYAttr, fillRotateZAttr, "fill", "fill", in, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputRotateAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(inputRotateAttr);
	addAttribute(fillRotateAttr);
	addAttribute(fillLogicallyAttr);
	addAttribute(sizeAttr);
	addAttribute(outputRotateAttr);

	attributeAffects(inputRotateAttr, outputRotateAttr);
	attributeAffects(inputRotateXAttr, outputRotateAttr);
	attributeAffects(inputRotateYAttr, outputRotateAttr);
	attributeAffects(inputRotateZAttr, outputRotateAttr);
	attributeAffects(fillRotateAttr, outputRotateAttr);
	attributeAffects(fillRotateXAttr, outputRotateAttr);
	attributeAffects(fillRotateYAttr, outputRotateAttr);
	attributeAffects(fillRotateZAttr, outputRotateAttr);
	attributeAffects(fillLogicallyAttr, outputRotateAttr);
	attributeAffects(sizeAttr, outputRotateAttr);

	return MStatus::kSuccess;
}

MStatus PackEulerArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputRotateAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	MEulerRotation fillRotate = inputEulerValue(dataBlock, fillRotateAttr, fillRotateXAttr, fillRotateYAttr, fillRotateZAttr);
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_rotations.resize(size);

	MArrayDataHandle inRotateArrayHandle = dataBlock.inputArrayValue(inputRotateAttr);

	unsigned int count = 0;
	unsigned int elementCount = std::min(inRotateArrayHandle.elementCount(), size);
	unsigned int elementIndex = 0;

	if (fillLogically)
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			if (inRotateArrayHandle.elementIndex() < size)
			{
				// Only record assigned indices as this value will be used to fill the end of the array if necessary
				elementIndex = inRotateArrayHandle.elementIndex();
				MDataHandle inRotateElementHandle = inRotateArrayHandle.inputValue();
				const double x = inRotateElementHandle.child(inputRotateXAttr).asAngle().asRadians();
				const double y = inRotateElementHandle.child(inputRotateYAttr).asAngle().asRadians();
				const double z = inRotateElementHandle.child(inputRotateZAttr).asAngle().asRadians();
				m_rotations[elementIndex] = MEulerRotation(x, y, z);

				while (count < elementIndex)
				{
					m_rotations[count] = fillRotate;
					count++;
				}

				count++;
				inRotateArrayHandle.next();
			}
		}

		// If the size is greater than the elementCount, fill the array from the next index if elements exist, otherwise fill the entire array starting from index 0
		// We check the count has been incremented at least once, otherwise the array should be filled from the start
		elementIndex = elementCount > 0 && count > 0 ? elementIndex + 1 : 0;
		for (unsigned int i = elementIndex; i < size; i++)
			m_rotations[i] = fillRotate;
	}
	else
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle inRotateElementHandle = inRotateArrayHandle.inputValue();
			const double x = inRotateElementHandle.child(inputRotateXAttr).asAngle().asRadians();
			const double y = inRotateElementHandle.child(inputRotateYAttr).asAngle().asRadians();
			const double z = inRotateElementHandle.child(inputRotateZAttr).asAngle().asRadians();
			m_rotations[i] = MEulerRotation(x, y, z);
			inRotateArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_rotations[i] = fillRotate;
	}

	outputPluginDataArrayValue<EulerArrayData>(dataBlock, outputRotateAttr, m_rotations);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------