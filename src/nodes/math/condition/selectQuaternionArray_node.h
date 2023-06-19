#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "data/quaternionArray_data.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SelectQuaternionArray : public MPxNode, MRS::NodeHelper
{
public:
	SelectQuaternionArray();
	~SelectQuaternionArray();

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