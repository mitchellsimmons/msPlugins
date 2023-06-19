#pragma once

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>
#include <maya/MMessage.h>
#include <maya/MDGModifier.h>
#include <maya/MNodeMessage.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// Node used to create a parent container for NodeBox items in the Node Editor
// It will not save with the scene (ie. it is used purely to interface with the Node Editor)
class NodeBoxContainer : public MPxNode, MRS::NodeHelper
{
public:
	NodeBoxContainer();
	~NodeBoxContainer();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	void postConstructor() override;
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;
};