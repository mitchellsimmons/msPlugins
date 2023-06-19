#include "rig_hrc.h"
#include "hrc_matrix.h"

/*	Description
	-----------
	A custom MPxTransform node designed to provide a non-transforming, hierarchical grouping within the DAG.
	Nodes of this type are designed specifically to interface with the mrs metadata system, acting as the root of a rig.
	Nodes of this type will ignore any attempt at local transformation. */

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RigHrc::RigHrc() : MPxTransform(), NodeHelper() {}
#if MAYA_API_VERSION < 20190000
RigHrc::RigHrc(MPxTransformationMatrix* p_mtx) : MPxTransform(p_mtx), NodeHelper() {}
#endif
RigHrc::~RigHrc() {}

// ------ MPxTransform ------
MPxTransform::SchedulingType RigHrc::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RigHrc::initialize()
{
	return MStatus::kSuccess;
}

MPxTransformationMatrix* RigHrc::createTransformationMatrix()
{
	return new HrcMatrix();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------