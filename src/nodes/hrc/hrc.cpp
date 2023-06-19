#include "hrc.h"
#include "hrc_matrix.h"

/*	Description
	-----------
	A custom MPxTransform node designed to provide a non-transforming, hierarchical grouping within the DAG.
	Nodes of this type will ignore any attempt at local transformation. */

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Hrc::Hrc() : MPxTransform(), NodeHelper() {}
#if MAYA_API_VERSION < 20190000
Hrc::Hrc(MPxTransformationMatrix* p_mtx) : MPxTransform(p_mtx), NodeHelper() {}
#endif
Hrc::~Hrc() {}

// ------ MPxTransform ------
MPxTransform::SchedulingType Hrc::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Hrc::initialize()
{
	return MStatus::kSuccess;
}

MPxTransformationMatrix* Hrc::createTransformationMatrix()
{
	return new HrcMatrix();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------