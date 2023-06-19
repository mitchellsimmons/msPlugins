#include "distanceBetweenPoints_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DistanceBetweenPoints::DistanceBetweenPoints() {}
DistanceBetweenPoints::~DistanceBetweenPoints() {}

// ------ Attr ------
MObject DistanceBetweenPoints::input1ParentAttr;
MObject DistanceBetweenPoints::input1ChildXAttr;
MObject DistanceBetweenPoints::input1ChildYAttr;
MObject DistanceBetweenPoints::input1ChildZAttr;
MObject DistanceBetweenPoints::input2ParentAttr;
MObject DistanceBetweenPoints::input2ChildXAttr;
MObject DistanceBetweenPoints::input2ChildYAttr;
MObject DistanceBetweenPoints::input2ChildZAttr;
MObject DistanceBetweenPoints::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DistanceBetweenPoints::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DistanceBetweenPoints::initialize()
{
	MVector vector;

	createVectorAttribute(input1ParentAttr, input1ChildXAttr, input1ChildYAttr, input1ChildZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2ParentAttr, input2ChildXAttr, input2ChildYAttr, input2ChildZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1ParentAttr);
	addAttribute(input2ParentAttr);
	addAttribute(outputAttr);

	attributeAffects(input1ParentAttr, outputAttr);
	attributeAffects(input2ParentAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DistanceBetweenPoints::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1ParentAttr);
	MVector input2 = inputVectorValue(dataBlock, input2ParentAttr);
	MVector delta = input1 - input2;

	outputDoubleValue(dataBlock, outputAttr, delta.length());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------