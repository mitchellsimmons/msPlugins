#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"
#include "data/quaternionArray_data.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackQuaternionArray : public MPxNode, MRS::NodeHelper
{
public:
	PackQuaternionArray();
	~PackQuaternionArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputQuaternionAttr;
	static MObject inputQuaternionXAttr;
	static MObject inputQuaternionYAttr;
	static MObject inputQuaternionZAttr;
	static MObject inputQuaternionWAttr;
	static MObject fillQuaternionAttr;
	static MObject fillQuaternionXAttr;
	static MObject fillQuaternionYAttr;
	static MObject fillQuaternionZAttr;
	static MObject fillQuaternionWAttr;
	static MObject fillLogicallyAttr;
	static MObject sizeAttr;
	static MObject outputQuaternionAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MQuaternion> m_quaternions;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------