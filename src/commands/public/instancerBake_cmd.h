#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <maya/MAnimControl.h>
#include <maya/MAnimCurveChange.h>
#include <maya/MArgDataBase.h>
#include <maya/MArgList.h>
#include <maya/MDagModifier.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTransform.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPxCommand.h>
#include <maya/MSelectionList.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MSyntax.h>
#include <maya/MTime.h>
#include <maya/MVectorArray.h>

#include "utils/name_utils.h"

struct Duplicate
{
	MDagPath duplicatePath;

	MVector initialPosition;
	MVector initialScale;
	MEulerRotation initialRotation;

	MObject translateXAnimCurveObj;
	MObject translateYAnimCurveObj;
	MObject translateZAnimCurveObj;
	MObject rotateXAnimCurveObj;
	MObject rotateYAnimCurveObj;
	MObject rotateZAnimCurveObj;
	MObject scaleXAnimCurveObj;
	MObject scaleYAnimCurveObj;
	MObject scaleZAnimCurveObj;
	MObject rotateOrderAnimCurveObj;
	MObject visibilityAnimCurveObj;
};

class InstancerBake : public MPxCommand
{
public:
	InstancerBake();
	~InstancerBake() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ Const ------
	static const char* kStartTimeFlag;
	static const char* kStartTimeFlagLong;
	static const char* kEndTimeFlag;
	static const char* kEndTimeFlagLong;
	static const char* kTimeStepFlag;
	static const char* kTimeStepFlagLong;
	static const char* kInstanceFlag;
	static const char* kInstanceFlagLong;
	static const char* kParentFlag;
	static const char* kParentFlagLong;
	static const char* kWorldFlag;
	static const char* kWorldFlagLong;
	static const char* kInputSpaceFlag;
	static const char* kInputSpaceFlagLong;
	static const char* kDeleteInstancerFlag;
	static const char* kDeleteInstancerFlagLong;
	static const char* kDeleteSourceHierarchiesFlag;
	static const char* kDeleteSourceHierarchiesFlagLong;

	// ------ MPxCommand ------
	bool isUndoable() const override;
	MStatus doIt(const MArgList&) override;
	MStatus redoIt() override;
	MStatus undoIt() override;

private:
	MDagModifier m_dagMod;
	MAnimCurveChange m_animChangeMod;
};