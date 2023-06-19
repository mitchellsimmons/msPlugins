#pragma once

#include <algorithm>

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"
#include "data/angleArray_data.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackAngleArray : public MPxNode, MRS::NodeHelper
{
public:
	PackAngleArray();
	~PackAngleArray();

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
	std::vector<MAngle> m_data;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------