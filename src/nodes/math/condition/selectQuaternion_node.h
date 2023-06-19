#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SelectQuaternion : public MPxNode, MRS::NodeHelper
{
public:
	SelectQuaternion();
	~SelectQuaternion();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject ifFalseAttr;
	static MObject ifFalseXAttr;
	static MObject ifFalseYAttr;
	static MObject ifFalseZAttr;
	static MObject ifFalseWAttr;
	static MObject ifTrueAttr;
	static MObject ifTrueXAttr;
	static MObject ifTrueYAttr;
	static MObject ifTrueZAttr;
	static MObject ifTrueWAttr;
	static MObject conditionAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
	static MObject outputWAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------