#pragma once

#include <algorithm>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/math_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class FootRoll : public MPxNode, MRS::NodeHelper
{
public:
	FootRoll();
	~FootRoll();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject rollAttr;
	static MObject heelBackDriverAttr;
	static MObject tarsusLockDriverAttr;
	static MObject tarsusLockedDriverAttr;
	static MObject tarsusStraightenDriverAttr;
	static MObject tarsusLimitAttr;
	static MObject toeLimitAttr;
	static MObject tarsusStraightenFactorAttr;
	static MObject heelSmoothFactorAttr;
	static MObject tarsusSmoothFactorAttr;
	static MObject toeSmoothFactorAttr;
	static MObject outputTarsusAttr;
	static MObject outputToeAttr;
	static MObject outputHeelAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------