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

class ArithmeticSequenceVectorArray : public MPxNode, MRS::NodeHelper
{
public:
	ArithmeticSequenceVectorArray();
	~ArithmeticSequenceVectorArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject firstTermAttr;
	static MObject firstTermXAttr;
	static MObject firstTermYAttr;
	static MObject firstTermZAttr;
	static MObject commonDifferenceAttr;
	static MObject commonDifferenceXAttr;
	static MObject commonDifferenceYAttr;
	static MObject commonDifferenceZAttr;
	static MObject sizeAttr;
	static MObject outputSequenceAttr;

private:
	// ------ Data ------
	std::vector<MVector> m_outputSequence;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------