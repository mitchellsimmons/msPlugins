#pragma once

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

// Note, animating this node with linear tangents is exactly the same as animating the Lerp node with auto tangents (ie. redundant unless static sampling)
class SmoothstepAngle : public MPxNode, MRS::NodeHelper
{
public:
	SmoothstepAngle();
	~SmoothstepAngle();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject input1Attr;
	static MObject input2Attr;
	static MObject tAttr;
	static MObject outputAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------