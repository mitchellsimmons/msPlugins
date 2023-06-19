#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackMatrixArray : public MPxNode, MRS::NodeHelper
{
public:
	PackMatrixArray();
	~PackMatrixArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputMatrixAttr;
	static MObject fillMatrixAttr;
	static MObject fillLogicallyAttr;
	static MObject sizeAttr;
	static MObject outputMatrixAttr;
	
	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	MMatrixArray m_matrices;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------