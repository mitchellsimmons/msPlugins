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

class SlerpQuaternion : public MPxNode, MRS::NodeHelper
{
public:
	SlerpQuaternion();
	~SlerpQuaternion();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject input1Attr;
	static MObject input1XAttr;
	static MObject input1YAttr;
	static MObject input1ZAttr;
	static MObject input1WAttr;
	static MObject input2Attr;
	static MObject input2XAttr;
	static MObject input2YAttr;
	static MObject input2ZAttr;
	static MObject input2WAttr;
	static MObject tAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
	static MObject outputWAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------