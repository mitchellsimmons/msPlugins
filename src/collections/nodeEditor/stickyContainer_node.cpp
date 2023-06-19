#include "stickyContainer_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

StickyContainer::StickyContainer() {}
StickyContainer::~StickyContainer() {}

// ------ MPxNode ------
MPxNode::SchedulingType StickyContainer::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus StickyContainer::initialize()
{
	return MStatus::kSuccess;
}

void StickyContainer::postConstructor()
{
	setDoNotWrite(true);
}

MStatus StickyContainer::compute(const MPlug &plug, MDataBlock &dataBlock)
{
	return MStatus::kUnknownParameter;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------