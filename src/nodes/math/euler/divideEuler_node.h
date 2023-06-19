#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/math_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DivideEuler : public MPxNode, MRS::NodeHelper
{
public:
	DivideEuler();
	~DivideEuler();

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
	static MObject input2Attr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------