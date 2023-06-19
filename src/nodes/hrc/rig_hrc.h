#pragma once

#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class RigHrc : public MPxTransform, MRS::NodeHelper
{
public:
	RigHrc();
#if MAYA_API_VERSION < 20190000
	RigHrc(MPxTransformationMatrix* p_mtx);
#endif
	~RigHrc();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxTransform ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MPxTransformationMatrix* createTransformationMatrix() override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------