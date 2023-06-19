#pragma once

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "data/angleArray_data.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ArithmeticSequenceAngleArray : public MPxNode, MRS::NodeHelper
{
public:
	ArithmeticSequenceAngleArray();
	~ArithmeticSequenceAngleArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject firstTermAttr;
	static MObject commonDifferenceAttr;
	static MObject sizeAttr;
	static MObject outputSequenceAttr;

private:
	// ------ Data ------
	std::vector<MAngle> m_outputSequence;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------