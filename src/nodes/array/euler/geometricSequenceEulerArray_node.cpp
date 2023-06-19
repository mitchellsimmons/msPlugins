#include "geometricSequenceEulerArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GeometricSequenceEulerArray::GeometricSequenceEulerArray() {}
GeometricSequenceEulerArray::~GeometricSequenceEulerArray() {}

// ------ Attr ------
MObject GeometricSequenceEulerArray::firstTermAttr;
MObject GeometricSequenceEulerArray::firstTermXAttr;
MObject GeometricSequenceEulerArray::firstTermYAttr;
MObject GeometricSequenceEulerArray::firstTermZAttr;
MObject GeometricSequenceEulerArray::commonRatioAttr;
MObject GeometricSequenceEulerArray::commonRatioXAttr;
MObject GeometricSequenceEulerArray::commonRatioYAttr;
MObject GeometricSequenceEulerArray::commonRatioZAttr;
MObject GeometricSequenceEulerArray::sizeAttr;
MObject GeometricSequenceEulerArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType GeometricSequenceEulerArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus GeometricSequenceEulerArray::initialize()
{
	std::vector<MEulerRotation> outputs;
	MEulerRotation firstTerm;
	MVector commonRatio;

	createEulerAttribute(firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr,
		"firstTerm", "firstTerm", firstTerm, kDefaultPreset | kKeyable);
	createVectorAttribute(commonRatioAttr, commonRatioXAttr, commonRatioYAttr, commonRatioZAttr,
		"commonRatio", "commonRatio", commonRatio, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonRatioAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonRatioAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus GeometricSequenceEulerArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation a = inputEulerValue(dataBlock, firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr);
	MVector r = inputVectorValue(dataBlock, commonRatioAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
	{
		MEulerRotation element = m_outputSequence[n - 1];
		element.x *= r.x;
		element.y *= r.y;
		element.z *= r.z;
		m_outputSequence[n] = element;
	}

	outputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------