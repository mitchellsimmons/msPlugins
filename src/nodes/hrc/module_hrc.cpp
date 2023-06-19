#include "module_hrc.h"
#include "hrc_matrix.h"

/*	Description
	-----------
	A custom MPxTransform node designed to provide a non-transforming, hierarchical grouping within the DAG.
	Nodes of this type are designed specifically to interface with the mrs metadata system, acting as the root of a module.
	Nodes of this type will ignore any attempt at local transformation. */

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ModuleHrc::ModuleHrc() : MPxTransform(), NodeHelper() {}
#if MAYA_API_VERSION < 20190000
ModuleHrc::ModuleHrc(MPxTransformationMatrix* p_mtx) : MPxTransform(p_mtx), NodeHelper() {}
#endif
ModuleHrc::~ModuleHrc() {}

// ------ MPxTransform ------
MPxTransform::SchedulingType ModuleHrc::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ModuleHrc::initialize()
{
	return MStatus::kSuccess;
}

MPxTransformationMatrix* ModuleHrc::createTransformationMatrix()
{
	return new HrcMatrix();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------