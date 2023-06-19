#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DistanceBetweenPoints : public MPxNode, MRS::NodeHelper
{
public:
	DistanceBetweenPoints();
	~DistanceBetweenPoints();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject input1ParentAttr;
	static MObject input1ChildXAttr;
	static MObject input1ChildYAttr;
	static MObject input1ChildZAttr;
	static MObject input2ParentAttr;
	static MObject input2ChildXAttr;
	static MObject input2ChildYAttr;
	static MObject input2ChildZAttr;
	static MObject outputAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------