#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SignedAngleBetweenVectors : public MPxNode, MRS::NodeHelper
{
public:
	SignedAngleBetweenVectors();
	~SignedAngleBetweenVectors();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject unitAxisAttr;
	static MObject unitAxisXAttr;
	static MObject unitAxisYAttr;
	static MObject unitAxisZAttr;
	static MObject input1Attr;
	static MObject input1XAttr;
	static MObject input1YAttr;
	static MObject input1ZAttr;
	static MObject input2Attr;
	static MObject input2XAttr;
	static MObject input2YAttr;
	static MObject input2ZAttr;
	static MObject outputAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------