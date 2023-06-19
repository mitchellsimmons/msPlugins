#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class MultiplyMatrix : public MPxNode, MRS::NodeHelper
{
public:
	MultiplyMatrix();
	~MultiplyMatrix();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject input1Attr;
	static MObject input2Attr;
	static MObject outputAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------