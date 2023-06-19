#pragma once

#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
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
#include "data/quaternionArray_data.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class UnpackQuaternionArray : public MPxNode, MRS::NodeHelper
{
public:
	UnpackQuaternionArray();
	~UnpackQuaternionArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputQuaternionAttr;
	static MObject outputQuaternionAttr;
	static MObject outputQuaternionXAttr;
	static MObject outputQuaternionYAttr;
	static MObject outputQuaternionZAttr;
	static MObject outputQuaternionWAttr;
	
	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MQuaternion> m_quaternions;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------