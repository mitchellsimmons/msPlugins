#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackDoubleArray : public MPxNode, MRS::NodeHelper
{
public:
	PackDoubleArray();
	~PackDoubleArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;
	
	// ------ Attr ------
	static MObject inputAttr;
	static MObject fillAttr;
	static MObject fillLogicallyAttr;
	static MObject sizeAttr;
	static MObject outputAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	MDoubleArray m_data;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------