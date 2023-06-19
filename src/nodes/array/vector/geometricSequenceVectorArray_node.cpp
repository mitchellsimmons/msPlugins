#include "geometricSequenceVectorArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GeometricSequenceVectorArray::GeometricSequenceVectorArray() {}
GeometricSequenceVectorArray::~GeometricSequenceVectorArray() {}

// ------ Attr ------
MObject GeometricSequenceVectorArray::firstTermAttr;
MObject GeometricSequenceVectorArray::firstTermXAttr;
MObject GeometricSequenceVectorArray::firstTermYAttr;
MObject GeometricSequenceVectorArray::firstTermZAttr;
MObject GeometricSequenceVectorArray::commonRatioAttr;
MObject GeometricSequenceVectorArray::commonRatioXAttr;
MObject GeometricSequenceVectorArray::commonRatioYAttr;
MObject GeometricSequenceVectorArray::commonRatioZAttr;
MObject GeometricSequenceVectorArray::sizeAttr;
MObject GeometricSequenceVectorArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType GeometricSequenceVectorArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus GeometricSequenceVectorArray::initialize()
{
	std::vector<MVector> outputs;
	MVector firstTerm;
	MVector commonRatio;

	createVectorAttribute(firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr,
		"firstTerm", "firstTerm", firstTerm, kDefaultPreset | kKeyable);
	createVectorAttribute(commonRatioAttr, commonRatioXAttr, commonRatioYAttr, commonRatioZAttr,
		"commonRatio", "commonRatio", commonRatio, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createVectorDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonRatioAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonRatioAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus GeometricSequenceVectorArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MVector a = inputVectorValue(dataBlock, firstTermAttr);
	MVector r = inputVectorValue(dataBlock, commonRatioAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
	{
		MVector element = m_outputSequence[n - 1];
		element.x *= r.x;
		element.y *= r.y;
		element.z *= r.z;
		m_outputSequence[n] = element;
	}

	outputVectorDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------