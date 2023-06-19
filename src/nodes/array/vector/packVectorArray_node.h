#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackVectorArray : public MPxNode, MRS::NodeHelper
{
public:
	PackVectorArray();
	~PackVectorArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputVectorAttr;
	static MObject inputVectorXAttr;
	static MObject inputVectorYAttr;
	static MObject inputVectorZAttr;
	static MObject fillVectorAttr;
	static MObject fillVectorXAttr;
	static MObject fillVectorYAttr;
	static MObject fillVectorZAttr;
	static MObject fillLogicallyAttr;
	static MObject sizeAttr;
	static MObject outputVectorAttr;
	
	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	MVectorArray m_vectors;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------