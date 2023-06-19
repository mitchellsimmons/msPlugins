#include "packVectorArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PackVectorArray::PackVectorArray() : MPxNode(), NodeHelper() {}
PackVectorArray::~PackVectorArray() {}

// ------ Attr ------
MObject PackVectorArray::inputVectorAttr;
MObject PackVectorArray::inputVectorXAttr;
MObject PackVectorArray::inputVectorYAttr;
MObject PackVectorArray::inputVectorZAttr;
MObject PackVectorArray::fillVectorAttr;
MObject PackVectorArray::fillVectorXAttr;
MObject PackVectorArray::fillVectorYAttr;
MObject PackVectorArray::fillVectorZAttr;
MObject PackVectorArray::fillLogicallyAttr;
MObject PackVectorArray::sizeAttr;
MObject PackVectorArray::outputVectorAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PackVectorArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PackVectorArray::initialize()
{
	MVector in;
	std::vector<MVector> out;

	createVectorAttribute(inputVectorAttr, inputVectorXAttr, inputVectorYAttr, inputVectorZAttr, "input", "input", in, kArrayPreset | kKeyable);
	createVectorAttribute(fillVectorAttr, fillVectorXAttr, fillVectorYAttr, fillVectorZAttr, "fill", "fill", in, kDefaultPreset | kKeyable);
	createBoolAttribute(fillLogicallyAttr, "fillLogically", "fillLogically", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createVectorDataArrayAttribute(outputVectorAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(inputVectorAttr);
	addAttribute(fillVectorAttr);
	addAttribute(fillLogicallyAttr);
	addAttribute(sizeAttr);
	addAttribute(outputVectorAttr);

	attributeAffects(inputVectorAttr, outputVectorAttr);
	attributeAffects(inputVectorXAttr, outputVectorAttr);
	attributeAffects(inputVectorYAttr, outputVectorAttr);
	attributeAffects(inputVectorZAttr, outputVectorAttr);
	attributeAffects(fillVectorAttr, outputVectorAttr);
	attributeAffects(fillVectorXAttr, outputVectorAttr);
	attributeAffects(fillVectorYAttr, outputVectorAttr);
	attributeAffects(fillVectorZAttr, outputVectorAttr);
	attributeAffects(fillLogicallyAttr, outputVectorAttr);
	attributeAffects(sizeAttr, outputVectorAttr);

	return MStatus::kSuccess;
}

MStatus PackVectorArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputVectorAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = dataBlock.inputValue(sizeAttr).asInt();
	MVector fillVector = dataBlock.inputValue(fillVectorAttr).asVector();
	bool fillLogically = inputBoolValue(dataBlock, fillLogicallyAttr);
	m_vectors.setLength(size);

	MArrayDataHandle inVectorArrayHandle = dataBlock.inputArrayValue(inputVectorAttr);
	MDataHandle outVectorDataArrayHandle = dataBlock.outputValue(outputVectorAttr);

	unsigned int count = 0;
	unsigned int elementCount = std::min(inVectorArrayHandle.elementCount(), size);
	unsigned int elementIndex = 0;

	if (fillLogically)
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			if (inVectorArrayHandle.elementIndex() < size)
			{
				// Only record assigned indices as this value will be used to fill the end of the array if necessary
				elementIndex = inVectorArrayHandle.elementIndex();
				MDataHandle inVectorElementHandle = inVectorArrayHandle.inputValue();
				const double x = inVectorElementHandle.child(inputVectorXAttr).asDouble();
				const double y = inVectorElementHandle.child(inputVectorYAttr).asDouble();
				const double z = inVectorElementHandle.child(inputVectorZAttr).asDouble();
				m_vectors[elementIndex] = MVector(x, y, z);

				while (count < elementIndex)
				{
					m_vectors[count] = fillVector;
					count++;
				}

				count++;
				inVectorArrayHandle.next();
			}
		}

		// If the size is greater than the elementCount, fill the array from the next index if elements exist, otherwise fill the entire array starting from index 0
		// We check the count has been incremented at least once, otherwise the array should be filled from the start
		elementIndex = elementCount > 0 && count > 0 ? elementIndex + 1 : 0;
		for (unsigned int i = elementIndex; i < size; i++)
			m_vectors[i] = fillVector;
	}
	else
	{
		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle inVectorElementHandle = inVectorArrayHandle.inputValue();
			const double x = inVectorElementHandle.child(inputVectorXAttr).asDouble();
			const double y = inVectorElementHandle.child(inputVectorYAttr).asDouble();
			const double z = inVectorElementHandle.child(inputVectorZAttr).asDouble();
			m_vectors[i] = MVector(x, y, z);
			inVectorArrayHandle.next();
		}

		for (unsigned int i = elementCount; i < size; i++)
			m_vectors[i] = fillVector;
	}

	MFnVectorArrayData fnVector;
	MObject outputObj = fnVector.create(m_vectors);
	outVectorDataArrayHandle.setMObject(outputObj);
	outVectorDataArrayHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------