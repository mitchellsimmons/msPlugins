#include "nodeBoxContainer_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NodeBoxContainer::NodeBoxContainer() {}
NodeBoxContainer::~NodeBoxContainer() {}

// ------ MPxNode ------
MPxNode::SchedulingType NodeBoxContainer::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NodeBoxContainer::initialize()
{
	return MStatus::kSuccess;
}

void NodeBoxContainer::postConstructor()
{
	setDoNotWrite(true);
}

MStatus NodeBoxContainer::compute(const MPlug &plug, MDataBlock &dataBlock)
{
	return MStatus::kUnknownParameter;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------