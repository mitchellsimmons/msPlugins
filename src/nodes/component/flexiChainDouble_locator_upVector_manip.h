#pragma once

#include <algorithm>

#include <maya/MDagPath.h>
#include <maya/MDGModifier.h>
#include <maya/MEvent.h>
#include <maya/MEventMessage.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDirectionManip.h>
#include <maya/MFnDistanceManip.h>
#include <maya/MFnTransform.h>
#include <maya/MFnNumericData.h>
#include <maya/MGLFunctionTable.h>
#include <maya/MGlobal.h>
#include <maya/MHardwareRenderer.h>
#include <maya/MHWGeometry.h>
#include <maya/MItSelectionList.h>
#include <maya/MModelMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxCommand.h>
#include <maya/MPxContext.h>
#include <maya/MPxContextCommand.h>
#include <maya/MPxManipContainer.h>
#include <maya/MPxSelectionContext.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/command_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class FlexiChainDouble_UpVectorManip : public MPxManipulatorNode
{
public:
	FlexiChainDouble_UpVectorManip();
	~FlexiChainDouble_UpVectorManip() override;

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode------
	void postConstructor() override;

	// ------ MPxManipulatorNode ------
	static MStatus initialize();
	MStatus connectToDependNode(const MObject& node);

	void preDrawUI(const M3dView& view) override;
	void drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const override;

	MStatus doPress(M3dView& view) override;
	MStatus doDrag(M3dView& view) override;
	MStatus doRelease(M3dView& view) override;
	MStatus doMove(M3dView& view, bool& refresh) override;

	// ------ Transient Data ------
	static unsigned int positionIndex;

	// ------ Helpers ------
	MStatus getCompoundPlug(MPlug& compoundPlug);
	MStatus getDirectionPlug(MPlug& directionPlug);
	MStatus getStatePlug(MPlug& statePlug);

	MString getCompoundPlugPartialName();

	bool isEnabled();
	bool isUserInteracting();
	bool isMouseHovered();
	bool isActive();

private:
	friend class FlexiChainDouble_UpVectorContext;

	// ------ Data ------
	FlexiChainDouble* m_locator;
	MDagPath m_locatorPath;

	// Start/end index
	unsigned int m_positionIndex;

	// Manip value indices
	int m_directionIndex;

	// GL names
	MGLuint m_activeName;
	MGLuint m_handleName;
	
	// Drawing
	bool m_isEnabled;
	MPoint m_pLineStart;
	MPoint m_pLineEnd;
	static const MDistance m_handleRadius;
	static const float m_tickPoints[][4];
	static const float m_crossPoints[][4];

	// Interaction
	MPoint m_pMouseStartHandle;
	MPoint m_pMouseEndHandle;
	MVector m_vInterPlaneNormalHandle;
	MVector m_vMousePressOffsetHandle;

	bool m_isUserInteracting;
	bool m_isMouseHovered;
	bool m_isActive;

	// ------ Helpers ------
	MPoint sampleCurve(double param);

	MPoint getCameraPosition(M3dView& view) const;
	MPoint getCameraPosition(const MHWRender::MFrameContext& frameContext) const;
	MVector getCameraAimDirection(M3dView& view) const;
	MVector getCameraAimDirection(const MHWRender::MFrameContext& frameContext) const;
	MVector getCameraUpDirection(M3dView& view) const;
	MVector getCameraUpDirection(const MHWRender::MFrameContext& frameContext) const;

	bool computeMouseIntersection(const MPoint& pMouse, const MVector& vMouseDirection, const MPoint& pPointOnInterPlane,
		const MVector& vInterPlaneNormal, MPoint& pIntersection);
	bool computeCurrentMouseIntersection(const MPoint& pPointOnInterPlane, const MVector& vInterPlaneNormal, MPoint& pIntersection);

	MStatus updatePlugDirection(MVector& vDirection);

	void updateManipState();
	void updateManipPoints();
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines the manipulator that is used when the tool context becomes active
class FlexiChainDouble_UpVectorManipContainer : public MPxManipContainer
{
public:
	FlexiChainDouble_UpVectorManipContainer();
	~FlexiChainDouble_UpVectorManipContainer() override;

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxManipContainer ------
	static MStatus initialize();
	MStatus createChildren() override;
	MStatus connectToDependNode(const MObject& node) override;

private:
	friend class FlexiChainDouble_UpVectorContext;

	// ------ Data ------
	FlexiChainDouble_UpVectorManip* m_manip;
	FlexiChainDouble* m_locator;
	MDagPath m_locatorPath;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a tool context that is responsible for creating and handling instances of the above manipulator
// A tool context is a custom event handler which is used to process mouse interaction (eg. override press/drag/release events)
class FlexiChainDouble_UpVectorContext : public MPxSelectionContext
{
public:
	FlexiChainDouble_UpVectorContext();
	~FlexiChainDouble_UpVectorContext() override;

	// ------ MPxContext ------
	void toolOnSetup(MEvent& event) override;
	void toolOffCleanup() override;
	MStatus doEnterRegion(MEvent& event) override;
	MStatus drawFeedback(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) override;

	// ------ Helpers ------
	MString getHoverManipPlug();
	MString getActiveManipPlug();
	void setActiveManipPlug(MString& activeManipPlugName);

private:
	// ------ Helpers ------
	static void updateManipulatorsCallback(void* data);
	static void nodePlugChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* data);

	// ------ Data ------
	unsigned int m_numActiveNodes;
	MCallbackId m_selectionChangedCallbackId;
	std::vector<MCallbackId> m_nodePlugChangedCallbackIds;
	std::vector<FlexiChainDouble_UpVectorManipContainer*> m_manipContainers;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a command that will be used to create a single instance of the above context
class FlexiChainDouble_UpVectorContextCommand : public MPxContextCommand
{
public:
	FlexiChainDouble_UpVectorContextCommand();
	~FlexiChainDouble_UpVectorContextCommand() override;

	// ------ Const ------
	static const MString kCommandName;
	static const char* kHoverFlag;
	static const char* kHoverFlagLong;
	static const char* kActiveFlag;
	static const char* kActiveFlagLong;

	// ------ MPxContextCommand ------
	MPxContext * makeObj() override;
	MStatus doEditFlags() override;
	MStatus doQueryFlags() override;
	MStatus appendSyntax() override;

private:
	// ------ Data ------
	FlexiChainDouble_UpVectorContext* m_context;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------