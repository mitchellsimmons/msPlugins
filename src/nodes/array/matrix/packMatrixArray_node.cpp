#include "packMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackMatrixArray::PackMatrixArray() : MPxNode(), NodeHelper() {}
PackMatrixArray::~PackMatrixArray() {}

// ------ Attr ------
MObject PackMatrixArray::inputMatrixAttr;
MObject PackMatrixArray::fillMatrixAttr;
MObject PackMatrixArray::fillLogicallyAttr;
MObject PackMatrixArray::sizeAttr;
MObject PackMatrixArray::outputMatrixAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackMatrixArray::initialize()
{
	MMatrix in;
	std::vector<MMatrix> out;

	createMatrixAttribute(inputMatrixAttr, "input", "input", in, kArrayPreset | kKeyable);
	createMatrixAttribute(fillMatrixAttr, "fill", "fill", in, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createMatrixDataArrayAttribute(outputMatrixAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(inputMatrixAttr);
	addAttribute(fillMatrixAttr);
	addAttribute(fillLogicallyAttr);
	addAttribute(sizeAttr);
	addAttribute(outputMatrixAttr);

	attributeAffects(inputMatrixAttr, outputMatrixAttr);
	attributeAffects(fillMatrixAttr, outputMatrixAttr);
	attributeAffects(fillLogicallyAttr, outputMatrixAttr);
	attributeAffects(sizeAttr, outputMatrixAttr);

	return MStatus::kSuccess;
}

MStatus PackMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputMatrixAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	MMatrix fillMatrix = dataBlock.inputValue(fillMatrixAttr).asMatrix();
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_matrices.setLength(size);

	MArrayDataHandle inMatrixArrayHandle = dataBlock.inputArrayValue(inputMatrixAttr);
	MDataHandle outMatrixDataArrayHandle = dataBlock.outputValue(outputMatrixAttr);

	unsigned int count = 0;
	unsigned int elementCount = std::min(inMatrixArrayHandle.elementCount(), size);
	unsigned int elementIndex = 0;

	if (fillLogically)
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			if (inMatrixArrayHandle.elementIndex() < size)
			{
				// Only record assigned indices as this value will be used to fill the end of the array if necessary
				elementIndex = inMatrixArrayHandle.elementIndex();
				MDataHandle inMatrixElementHandle = inMatrixArrayHandle.inputValue();
				m_matrices[elementIndex] = inMatrixElementHandle.asMatrix();

				while (count < elementIndex)
				{
					m_matrices[count] = fillMatrix;
					count++;
				}

				count++;
				inMatrixArrayHandle.next();
			}
		}

		// If the size is greater than the elementCount, fill the array from the next index if elements exist, otherwise fill the entire array starting from index 0
		// We check the count has been incremented at least once, otherwise the array should be filled from the start
		elementIndex = elementCount > 0 && count > 0 ? elementIndex + 1 : 0;
		for (unsigned int i = elementIndex; i < size; i++)
			m_matrices[i] = fillMatrix;
	}
	else
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle inMatrixElementHandle = inMatrixArrayHandle.inputValue();
			m_matrices[i] = inMatrixElementHandle.asMatrix();
			inMatrixArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_matrices[i] = fillMatrix;
	}

	MFnMatrixArrayData fnMatrix;
	MObject outputObj = fnMatrix.create(m_matrices);
	outMatrixDataArrayHandle.setMObject(outputObj);
	outMatrixDataArrayHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------