#include "geometricSequenceDoubleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GeometricSequenceDoubleArray::GeometricSequenceDoubleArray() {}
GeometricSequenceDoubleArray::~GeometricSequenceDoubleArray() {}

// ------ Attr ------
MObject GeometricSequenceDoubleArray::firstTermAttr;
MObject GeometricSequenceDoubleArray::commonRatioAttr;
MObject GeometricSequenceDoubleArray::sizeAttr;
MObject GeometricSequenceDoubleArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType GeometricSequenceDoubleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus GeometricSequenceDoubleArray::initialize()
{
	std::vector<double> outputs;

	createDoubleAttribute(firstTermAttr, "firstTerm", "firstTerm", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(commonRatioAttr, "commonRatio", "commonRatio", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createDoubleDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonRatioAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonRatioAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus GeometricSequenceDoubleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	double a = inputDoubleValue(dataBlock, firstTermAttr);
	double r = inputDoubleValue(dataBlock, commonRatioAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] * r;

	outputDoubleDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------