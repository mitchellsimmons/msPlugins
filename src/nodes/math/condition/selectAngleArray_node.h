#pragma once

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "data/angleArray_data.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SelectAngleArray : public MPxNode, MRS::NodeHelper
{
public:
	SelectAngleArray();
	~SelectAngleArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject ifFalseAttr;
	static MObject ifTrueAttr;
	static MObject conditionAttr;
	static MObject outputAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------