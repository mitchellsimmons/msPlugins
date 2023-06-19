/*	Description
	-----------
	This command bakes particle instances displayed by an instancer node into geometry which is visible in the DAG
	- It will either duplicate or instance the DAG hierarchies connected to the "inputHierarchy" array attribute of the given instancer node
	It uses the data from the "inputPoints" attribute to determine how many copies of each hierarchy should be generated and keyframed over the given time interval
	- This command dictates that the internal "id" and "position" arrays must exist even if they are empty
	- It relies upon the internal "id" array to determine which instances are visible at each frame, whilst the "position" array is a logical requirement
	- If the "id" array is not filled with unique ids, this command will fail to bake any instances
	- If the id of a given instance exists momentarily in the array, the visibility attribute of each hierarchy will be used to reflect this change in state

	Limitations
	-----------
	This command currently does not support cycling of input hierarchies
	- This includes when the "cycle" attribute is set to 0 (None) and the "objectIndex" of a particle changes during animation
	- This also includes when the "cycle" attribute is set to 1 (sequential)
	Instead, when a particle id is generated, it will use its initial "objectIndex" to determine which input hierarchy to duplicate
	- If an "objectIndex" has not been assigned to a particle, a value of 0 will be used (ie. the first connected input hierarchy will be duplicated)
	- The object assigned to a particle will not change for the entirety of the baking procedure

	MEL Command
	-----------
	InstancerBake [-startTime float] [-endTime float] [-timeStep float] [-instance boolean] [-parent string] [-world boolean]
		[-deleteInstancer boolean] [-deleteSourceHierarchies bool] [object]

	Flags
	-----
	-startTime(-st)
		This flag specifies the time at which the first keyframe will be set for the cache
		This flag is required

	-endTime(-et)
		This flag specifies the last possible time at which a keyframe may be set for the cache
		If the delta between the startTime and endTime is not evenly divisible by the timeStep, the last keyframe will occur before the endTime
		This flag is required

	-timeStep(-st)
		This flag specifies the interval at which keyframes will be set between the startTime (inclusive) and endTime (inclusive)
		A precautionary minimum value of 0.01 is enforced so that extremely small values do not tend towards an infinitely sized cached
		The default value is 1.0

	-instance (-i)
		This flag specifies whether the children of input hierarchies should be instanced (default is false)
		If true, any descendant of each root parent transform will either be directly or indirectly instanced depending on its ancestry

	-parent (-p)
		This flag specifies a parent for each of the duplicate hierarchies
		If this flag is not present and the -world flag is also not set, a new group will be created for the duplicate hierarchies

	-world (-w)
		If this flag is set, the duplicate hierarchies will be parented under the world
		If the parent flag is also set this flag will be ignored

	-inputeSpace (-is)
		This flag specifies the input hierarchy coordinate space which is to be inherited by each duplicate
		There are three possible values : object (0), local (1), world (2)
		The default space is local (0)

	-deleteInstancer (-di)
		This flag specifies whether the instancer node should be deleted after the duplicates have been created

	-deleteSourceHierarchies (-dsh)
		This flag specifies whether each of the original input hierarchies should be deleted after the duplicates have been created

	Args
	----
	object
		This argument should be used to specify the name of the instancer node for which this command is to bake particle instances    */

#include "instancerBake_cmd.h"

InstancerBake::InstancerBake() {}

InstancerBake::~InstancerBake() {}

// ------ Registration ------

const char* InstancerBake::kStartTimeFlag = "-st";
const char* InstancerBake::kStartTimeFlagLong = "-startTime";
const char* InstancerBake::kEndTimeFlag = "-et";
const char* InstancerBake::kEndTimeFlagLong = "-endTime";
const char* InstancerBake::kTimeStepFlag = "-ts";
const char* InstancerBake::kTimeStepFlagLong = "-timeStep";
const char* InstancerBake::kInstanceFlag = "-i";
const char* InstancerBake::kInstanceFlagLong = "-instance";
const char* InstancerBake::kParentFlag = "-p";
const char* InstancerBake::kParentFlagLong = "-parent";
const char* InstancerBake::kWorldFlag = "-w";
const char* InstancerBake::kWorldFlagLong = "-world";
const char* InstancerBake::kInputSpaceFlag = "-is";
const char* InstancerBake::kInputSpaceFlagLong = "-inputSpace";
const char* InstancerBake::kDeleteInstancerFlag = "-di";
const char* InstancerBake::kDeleteInstancerFlagLong = "-deleteInstancer";
const char* InstancerBake::kDeleteSourceHierarchiesFlag = "-dsh";
const char* InstancerBake::kDeleteSourceHierarchiesFlagLong = "-deleteSourceHierarchies";

MSyntax InstancerBake::newSyntax()
{
	MSyntax syntax;

	// Flags
	syntax.addFlag(kStartTimeFlag, kStartTimeFlagLong, MSyntax::kTime);
	syntax.addFlag(kEndTimeFlag, kEndTimeFlagLong, MSyntax::kTime);
	syntax.addFlag(kTimeStepFlag, kTimeStepFlagLong, MSyntax::kTime);
	syntax.addFlag(kInstanceFlag, kInstanceFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kParentFlag, kParentFlagLong, MSyntax::kString);
	syntax.addFlag(kWorldFlag, kWorldFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kInputSpaceFlag, kInputSpaceFlagLong, MSyntax::kLong);
	syntax.addFlag(kDeleteInstancerFlag, kDeleteInstancerFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kDeleteSourceHierarchiesFlag, kDeleteSourceHierarchiesFlagLong, MSyntax::kBoolean);

	// Args
	syntax.useSelectionAsDefault(true);
	syntax.setObjectType(MSyntax::kSelectionList, 1, 1);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

// ------ MPxCommand ------

#define kErrorFlagNotSet \
	"The \"-startTime\" and \"-endTime\" flags are required by this command."

#define kErrorParsingFlag \
	"Error parsing flag \"^1s\"."

#define kErrorInvalidTimeStep \
	"The \"-timeStep\" flag must be given a value greater or equal to 0.01 ."

#define kErrorInvalidTimeInterval \
	"The \"-endTime\" flag must be given a value greater than the \"-startTime\" flag"

#define kErrorNoValidObject \
	"This command requires a single instancer node to be specified or selected."

#define kErrorInvalidInstancer \
	"Object argument \"^1s\" does not exist or is not a valid instancer node."

#define kErrorInvalidParent \
	"Node \"^1s\" assigned to \"-parent\" flag does not exist or is not a valid transform."

#define kWarningInvalidConnection \
	"The connection \"^1s\" -> \"^2s\" does not represent a valid DAG node and will not contribute to the instancer bake."

#define kErrorInvalidInputSpace \
	"The \"-inputSpace\" flag must be given one of the following values: 0 (object space), 1 (local space), 2 (world space)."

#define kErrorParticleDataRequired \
	"Command requires internal \"^1s\" array on \"^2s.inputPoints\" to be set."

#define kErrorParticleDataIncorrectSize \
	"Internal \"^1s\" array exists but has been filled with an incorrect number of elements. Size of array must match value of \"^2s.instanceCount\"."

#define kErrorInvalidParticleObjectIndex \
	"Invalid object index ^1s, assigned to particle id ^2s. There are ^3s input hierarchies connected to \"^4s.inputHierarchy\""

bool InstancerBake::isUndoable() const
{
	return true;
}

MStatus InstancerBake::doIt(const MArgList& args)
{
	MStatus status;

	// Argument list parser
	MArgDatabase argParser(syntax(), args);

	// Check if required flags are set
	if (!argParser.isFlagSet(kStartTimeFlagLong) || !argParser.isFlagSet(kEndTimeFlagLong))
	{
		displayError(kErrorFlagNotSet);
		return MStatus::kFailure;
	}

	// Parse command flags
	MTime startTime;
	startTime.setUnit(MTime::uiUnit());
	if (!argParser.getFlagArgument(kStartTimeFlagLong, 0, startTime))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kStartTimeFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	MTime endTime;
	endTime.setUnit(MTime::uiUnit());
	if (!argParser.getFlagArgument(kEndTimeFlagLong, 0, endTime))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kEndTimeFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	MTime timeStep{ 1.0, MTime::uiUnit() };
	if (argParser.isFlagSet(kTimeStepFlagLong))
	{
		if (!argParser.getFlagArgument(kTimeStepFlagLong, 0, timeStep))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kTimeStepFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	bool instanceState = false;
	if (argParser.isFlagSet(kInstanceFlagLong))
	{
		if (!argParser.getFlagArgument(kInstanceFlagLong, 0, instanceState))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kInstanceFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	bool worldState = false;
	if (argParser.isFlagSet(kWorldFlagLong))
	{
		if (!argParser.getFlagArgument(kWorldFlagLong, 0, worldState))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kWorldFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	bool isParentSet = false;
	MString parent = "";
	if (argParser.isFlagSet(kParentFlagLong))
	{
		isParentSet = true;
		if (!argParser.getFlagArgument(kParentFlagLong, 0, parent))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kParentFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	int inputSpace = 0;
	if (argParser.isFlagSet(kInputSpaceFlagLong))
	{
		if (!argParser.getFlagArgument(kInputSpaceFlagLong, 0, inputSpace))
		{
			displayError(kErrorInvalidInputSpace);
			return MStatus::kFailure;
		}
	}
	bool deleteInstancerState = false;
	if (argParser.isFlagSet(kDeleteInstancerFlagLong))
	{
		if (!argParser.getFlagArgument(kDeleteInstancerFlagLong, 0, deleteInstancerState))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kDeleteInstancerFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	bool deleteSourceHierarchiesState = false;
	if (argParser.isFlagSet(kDeleteSourceHierarchiesFlagLong))
	{
		if (!argParser.getFlagArgument(kDeleteSourceHierarchiesFlagLong, 0, deleteSourceHierarchiesState))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kDeleteSourceHierarchiesFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}

	// Check parsed values are valid
	if (timeStep < 0.01)
	{
		displayError(kErrorInvalidTimeStep);
		return MStatus::kFailure;
	}

	if (endTime < startTime)
	{
		displayError(kErrorInvalidTimeInterval);
		return MStatus::kFailure;
	}

	MDagPath parentPath;
	if (isParentSet)
	{
		MSelectionList parentSelList;
		if (!parentSelList.add(parent) || !parentSelList.getDagPath(0, parentPath) || !parentPath.node().hasFn(MFn::kTransform))
		{
			MString msg;
			MString msgFormat = kErrorInvalidParent;
			msg.format(msgFormat, parent);
			displayError(msg);
			return MStatus::kFailure;
		}
	}

	// Parse specified object from either command arg or current selection
	MSelectionList selectionList;
	argParser.getObjects(selectionList);

	// Check if a node is selected
	if (selectionList.length() == 0)
	{
		displayError(kErrorNoValidObject);
		return MStatus::kFailure;
	}

	// Check if the first selected node is an instancer node
	MDagPath instancerPath;
	if (!selectionList.getDagPath(0, instancerPath) || !instancerPath.hasFn(MFn::kInstancer))
	{
		MString msg;
		MString msgFormat = kErrorInvalidInstancer;
		msg.format(msgFormat, instancerPath.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	// Get the path to the first instance of each transform connected to the instancer node's "inputHierarchy" plug
	MFnDependencyNode fnDepInstancer{ instancerPath.node() };
	MPlug inputHierarchyArrayPlug = fnDepInstancer.findPlug("inputHierarchy", false);
	MDagPathArray inputHierarchyPathArray;
	for (unsigned int i = 0; i < inputHierarchyArrayPlug.evaluateNumElements(); ++i)
	{
		MPlug inputHierarchyElementPlug = inputHierarchyArrayPlug.elementByPhysicalIndex(i);
		if (inputHierarchyElementPlug.isDestination())
		{
			MPlug connectedSourcePlug = inputHierarchyElementPlug.sourceWithConversion();
			// Instancer nodes should only allow connections to transforms but we will double check anyway
			if (connectedSourcePlug.node().hasFn(MFn::kTransform))
			{
				MDagPath connectedNodePath;
				MDagPath::getAPathTo(connectedSourcePlug.node(), connectedNodePath);
				inputHierarchyPathArray.append(connectedNodePath);
			}
			else
			{
				MString msg;
				MString msgFormat = kWarningInvalidConnection;
				msg.format(msgFormat, connectedSourcePlug.partialName(true, false, false, false, false, true), 
					inputHierarchyArrayPlug.partialName(true, false, false, false, false, true));
				displayWarning(msg);
			}
		}
	}

	// Get the short name of the instancer, we will use this to name duplicate input hierarchies
	MString instancerName = MRS::getShortName(instancerPath.partialPathName());
	// Get the short name of each root transform connected to the inputHierarchy attribute, we will also use these to name duplicates
	std::vector<MString> inputHierarchyNames;
	inputHierarchyNames.resize(inputHierarchyPathArray.length());
	for (unsigned int i = 0; i < inputHierarchyPathArray.length(); ++i)
		inputHierarchyNames[i] = MRS::getShortName(inputHierarchyPathArray[i].partialPathName());

	// Create a new transform if parent and world flags have not been used
	if (!isParentSet && !worldState)
	{
		MString parentName = instancerName + "_bake#";
		MString createNodeCmd = "createNode transform -name \"" + parentName + "\"";
		MString result = MGlobal::executeCommandStringResult(createNodeCmd, false, true);
		MSelectionList parentSelList;
		parentSelList.add(result);
		parentSelList.getDagPath(0, parentPath);
	}

	// Create aim node for calculating rotation
	MObject aimConstraintObj = m_dagMod.createNode("aimConstraint");
	m_dagMod.doIt();

	// Retrieve plugs
	MFnDependencyNode fnDepAimConstraint{ aimConstraintObj };
	MPlug targetPlug = fnDepAimConstraint.findPlug("target", false);
	targetPlug = targetPlug.elementByLogicalIndex(0);
	MPlug aimDirection = targetPlug.child(fnDepAimConstraint.attribute("targetTranslate"));
	MPlug aimDirectionXPlug = aimDirection.child(fnDepAimConstraint.attribute("targetTranslateX"));
	MPlug aimDirectionYPlug = aimDirection.child(fnDepAimConstraint.attribute("targetTranslateY"));
	MPlug aimDirectionZPlug = aimDirection.child(fnDepAimConstraint.attribute("targetTranslateZ"));
	MPlug aimAxisXPlug = fnDepAimConstraint.findPlug("aimVectorX", false);
	MPlug aimAxisYPlug = fnDepAimConstraint.findPlug("aimVectorY", false);
	MPlug aimAxisZPlug = fnDepAimConstraint.findPlug("aimVectorZ", false);
	MPlug aimWorldUpDirectionXPlug = fnDepAimConstraint.findPlug("worldUpVectorX", false);
	MPlug aimWorldUpDirectionYPlug = fnDepAimConstraint.findPlug("worldUpVectorY", false);
	MPlug aimWorldUpDirectionZPlug = fnDepAimConstraint.findPlug("worldUpVectorZ", false);
	MPlug aimUpAxisXPlug = fnDepAimConstraint.findPlug("upVectorX", false);
	MPlug aimUpAxisYPlug = fnDepAimConstraint.findPlug("upVectorY", false);
	MPlug aimUpAxisZPlug = fnDepAimConstraint.findPlug("upVectorZ", false);
	MPlug outRotateXPlug = fnDepAimConstraint.findPlug("constraintRotateX", false);
	MPlug outRotateYPlug = fnDepAimConstraint.findPlug("constraintRotateY", false);
	MPlug outRotateZPlug = fnDepAimConstraint.findPlug("constraintRotateZ", false);

	// Create/keyframe duplicates using data
	MTime currentTime = MAnimControl::currentTime();
	MTime iterTime = startTime;
	MPlug inputPointsPlug = fnDepInstancer.findPlug("inputPoints", false);
	MPlug instanceCountPlug = fnDepInstancer.findPlug("instanceCount", false);

	std::unordered_map<short, short> rotationOrderMap{ {0, 0}, { 1, 3 }, { 2, 4 }, { 3, 1 }, { 4, 2 }, { 5, 5 }};
	std::unordered_map<int, Duplicate> seenIdMap;
	std::unordered_set<int> hiddenIds;

	while (iterTime <= endTime)
	{
		MAnimControl::setCurrentTime(iterTime);

		// Get the particle data from the instancer node's "inputPoints" plug
		// Ensure each of the required internal arrays exists and contain the same number of elements
		MObject particleDataObj = inputPointsPlug.asMObject();
		MFnArrayAttrsData fnParticleData{ particleDataObj };
		unsigned int instanceCount = instanceCountPlug.asInt();

		// --- Required Data ---
		// ids
		MIntArray visibleIds;
		MFnArrayAttrsData::Type intType(MFnArrayAttrsData::kIntArray);
		if (fnParticleData.checkArrayExist("id", intType))
			visibleIds = fnParticleData.getIntData("id");
		else
		{
			MString msg;
			MString msgFormat = kErrorParticleDataRequired;
			msg.format(kErrorParticleDataRequired, "id", instancerPath.partialPathName());
			displayError(msg);
			return MStatus::kFailure;
		}
		if (visibleIds.length() != instanceCount)
		{
			MString msg;
			MString msgFormat = kErrorParticleDataIncorrectSize;
			msg.format(kErrorParticleDataIncorrectSize, "id", instancerPath.partialPathName());
			displayError(msg);
			return MStatus::kFailure;
		}

		// positions
		MVectorArray positions;
		MFnArrayAttrsData::Type vectorType(MFnArrayAttrsData::kVectorArray);
		if (fnParticleData.checkArrayExist("position", vectorType))
			positions = fnParticleData.getVectorData("position");
		else
		{
			MString msg;
			MString msgFormat = kErrorParticleDataRequired;
			msg.format(kErrorParticleDataRequired, "position", instancerPath.partialPathName());
			displayError(msg);
			return MStatus::kFailure;
		}
		if (positions.length() != instanceCount)
		{
			MString msg;
			MString msgFormat = kErrorParticleDataIncorrectSize;
			msg.format(kErrorParticleDataIncorrectSize, "position", instancerPath.partialPathName());
			displayError(msg);
			return MStatus::kFailure;
		}

		// --- Optional Data ---
		// object indices
		MIntArray objectIndices;
		if (fnParticleData.checkArrayExist("objectIndex", intType))
		{
			objectIndices = fnParticleData.getIntData("objectIndex");

			if (objectIndices.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "objectIndex", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// visibility
		MIntArray visibilities;
		if (fnParticleData.checkArrayExist("visibility", intType))
		{
			visibilities = fnParticleData.getIntData("visibility");

			if (visibilities.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "visibility", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// rotation type
		MIntArray rotationTypes;
		if (fnParticleData.checkArrayExist("rotationType", intType))
		{
			rotationTypes = fnParticleData.getIntData("rotationType");

			if (rotationTypes.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "rotationType", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// rotation
		MVectorArray rotations;
		if (fnParticleData.checkArrayExist("rotation", vectorType))
		{
			rotations = fnParticleData.getVectorData("rotation");

			if (rotations.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "rotation", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// aim direction
		MVectorArray aimDirections;
		if (fnParticleData.checkArrayExist("aimDirection", vectorType))
		{
			aimDirections = fnParticleData.getVectorData("aimDirection");

			if (aimDirections.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "aimDirection", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// aim position
		MVectorArray aimPositions;
		if (fnParticleData.checkArrayExist("aimPosition", vectorType))
		{
			aimPositions = fnParticleData.getVectorData("aimPosition");

			if (aimPositions.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "aimPosition", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// aim axis
		MVectorArray aimAxes;
		if (fnParticleData.checkArrayExist("aimAxes", vectorType))
		{
			aimAxes = fnParticleData.getVectorData("aimAxes");

			if (aimAxes.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "aimAxes", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// aim up axis
		MVectorArray aimUpAxes;
		if (fnParticleData.checkArrayExist("aimUpAxes", vectorType))
		{
			aimUpAxes = fnParticleData.getVectorData("aimUpAxes");

			if (aimUpAxes.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "aimUpAxes", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// aim world up axis
		MVectorArray aimWorldUpDirections;
		if (fnParticleData.checkArrayExist("aimWorldUp", vectorType))
		{
			aimWorldUpDirections = fnParticleData.getVectorData("aimWorldUp");

			if (aimWorldUpDirections.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "aimWorldUp", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// scale
		MVectorArray scales;
		if (fnParticleData.checkArrayExist("scale", vectorType))
		{
			scales = fnParticleData.getVectorData("scale");

			if (scales.length() != instanceCount)
			{
				MString msg;
				MString msgFormat = kErrorParticleDataIncorrectSize;
				msg.format(kErrorParticleDataIncorrectSize, "scale", instancerPath.partialPathName());
				displayError(msg);
				return MStatus::kFailure;
			}
		}

		// The hiddenIds will contain all the particle ids from previous frames which are no longer visible at the current frame
		hiddenIds.clear();
		for (auto it = seenIdMap.cbegin(); it != seenIdMap.cend(); ++it)
			hiddenIds.insert(it->first);

		for (unsigned int i = 0; i < visibleIds.length(); ++i)
		{
			// If the particle id has not been seen, create a new duplicate
			if (hiddenIds.erase(visibleIds[i]) == 0)
			{
				// We are currently not supporting cycling input hierarchies, therefore the objectIndex used to duplicate the geometry will not change
				// If we were to support cycling we would need to create a duplicate of each input hierarchy for each particle and cycle the visibility of each
				unsigned int objectIndex = 0;
				if (objectIndices.length())
					objectIndex = objectIndices[i];
				if (objectIndex >= inputHierarchyPathArray.length())
				{
					MString msg;
					MString msgFormat = kErrorInvalidParticleObjectIndex;
					msg.format(kErrorInvalidParticleObjectIndex, std::to_string(objectIndex).c_str(), std::to_string(visibleIds[i]).c_str(),
						std::to_string(inputHierarchyPathArray.length()).c_str(), instancerPath.partialPathName());
					displayError(msg);
					return MStatus::kFailure;
				}

				MString duplicateName = instancerName + "_particle" + MRS::getPaddedInt(std::abs(visibleIds[i])) + "_" + inputHierarchyNames[objectIndex];
				MString duplicateResult;
				if (instanceState)
				{
					MStringArray duplicates;
					MString duplicateCmd = "instance " + inputHierarchyPathArray[objectIndex].partialPathName();
					MGlobal::executeCommand(duplicateCmd, duplicates, false, true);
					duplicateResult = duplicates[0];
				}
				else
				{
					MStringArray duplicates;
					MString duplicateCmd = "duplicate " + inputHierarchyPathArray[objectIndex].partialPathName();
					MGlobal::executeCommand(duplicateCmd, duplicates, false, true);
					duplicateResult = duplicates[0];
				}

				if (parentPath.isValid())
				{
					MStringArray duplicates;
					MString parentCmd = "parent -relative " + duplicateResult + " " + parentPath.partialPathName();
					MGlobal::executeCommand(parentCmd, duplicates, false, true);
					duplicateResult = duplicates[0];
				}
				else if (std::string{ duplicateResult.asChar() }.find("|") != std::string::npos)
				{
					MString parentCmd = "parent -world -relative " + duplicateResult;
					duplicateResult = MGlobal::executeCommandStringResult(parentCmd, false, true);
				}

				MString renameCmd = "rename -ignoreShape " + duplicateResult + " " + duplicateName;
				duplicateResult = MGlobal::executeCommandStringResult(renameCmd, false, true);

				// Create a new struct to store duplicate data
				Duplicate duplicate;
				MSelectionList duplicateSelList;
				duplicateSelList.add(duplicateResult);
				duplicateSelList.getDagPath(0, duplicate.duplicatePath);
				MFnDependencyNode fnDepDuplicate{ duplicate.duplicatePath.node() };

				// Retrieve additional static transforms based on the inputSpace flag before connecting anim curves
				if (inputSpace == 0)
					duplicate.initialScale = { 1,1,1 };
				if (inputSpace == 1)
				{
					MFnTransform fnTransform{ duplicate.duplicatePath };
					duplicate.initialPosition = fnTransform.getTranslation(MSpace::kTransform);
					fnTransform.getRotation(duplicate.initialRotation);
					fnTransform.getScale(&duplicate.initialScale.x);
				}
				else if (inputSpace == 2)
				{
					MMatrix worldMatrix = duplicate.duplicatePath.inclusiveMatrix();
					MTransformationMatrix fnTransWorldMat{ worldMatrix };
					duplicate.initialPosition = { worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] };
					duplicate.initialRotation = fnTransWorldMat.eulerRotation();
					fnTransWorldMat.getScale(&duplicate.initialScale.x, MSpace::kTransform);
				}

				// Retrieve relevent keyable plugs
				MPlug translateXPlug = fnDepDuplicate.findPlug("translateX", false);
				MPlug translateYPlug = fnDepDuplicate.findPlug("translateY", false);
				MPlug translateZPlug = fnDepDuplicate.findPlug("translateZ", false);
				MPlug rotateXPlug = fnDepDuplicate.findPlug("rotateX", false);
				MPlug rotateYPlug = fnDepDuplicate.findPlug("rotateY", false);
				MPlug rotateZPlug = fnDepDuplicate.findPlug("rotateZ", false);
				MPlug scaleXPlug = fnDepDuplicate.findPlug("scaleX", false);
				MPlug scaleYPlug = fnDepDuplicate.findPlug("scaleY", false);
				MPlug scaleZPlug = fnDepDuplicate.findPlug("scaleZ", false);
				MPlug rotateOrderPlug = fnDepDuplicate.findPlug("rotateOrder", false);
				MPlug visibilityPlug = fnDepDuplicate.findPlug("visibility", false);

				// Create a new animCurve for each keyable attribute
				// MFnAnimCurve::create() will invoke doIt() on the modifier, creating a new anim curve and connecting it to the plug
				MFnAnimCurve fnAnimCurve;
				duplicate.translateXAnimCurveObj = fnAnimCurve.create(translateXPlug, MFnAnimCurve::kAnimCurveTL, &m_dagMod, &status);
				duplicate.translateYAnimCurveObj = fnAnimCurve.create(translateYPlug, MFnAnimCurve::kAnimCurveTL, &m_dagMod, &status);
				duplicate.translateZAnimCurveObj = fnAnimCurve.create(translateZPlug, MFnAnimCurve::kAnimCurveTL, &m_dagMod, &status);
				duplicate.rotateXAnimCurveObj = fnAnimCurve.create(rotateXPlug, MFnAnimCurve::kAnimCurveTA, &m_dagMod, &status);
				duplicate.rotateYAnimCurveObj = fnAnimCurve.create(rotateYPlug, MFnAnimCurve::kAnimCurveTA, &m_dagMod, &status);
				duplicate.rotateZAnimCurveObj = fnAnimCurve.create(rotateZPlug, MFnAnimCurve::kAnimCurveTA, &m_dagMod, &status);
				duplicate.scaleXAnimCurveObj = fnAnimCurve.create(scaleXPlug, MFnAnimCurve::kAnimCurveTU, &m_dagMod, &status);
				duplicate.scaleYAnimCurveObj = fnAnimCurve.create(scaleYPlug, MFnAnimCurve::kAnimCurveTU, &m_dagMod, &status);
				duplicate.scaleZAnimCurveObj = fnAnimCurve.create(scaleZPlug, MFnAnimCurve::kAnimCurveTU, &m_dagMod, &status);
				duplicate.rotateOrderAnimCurveObj = fnAnimCurve.create(rotateOrderPlug, MFnAnimCurve::kAnimCurveTU, &m_dagMod, &status);
				duplicate.visibilityAnimCurveObj = fnAnimCurve.create(visibilityPlug, MFnAnimCurve::kAnimCurveTU, &m_dagMod, &status);

				// If the duplicate is visible when created, we want it to be invisible on all preceeding frames
				if (iterTime != startTime)
					fnAnimCurve.addKey(iterTime - timeStep, false, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);

				// Cache data
				seenIdMap.insert(std::pair<int, Duplicate>{ visibleIds[i], duplicate });
			}

			// Update the transforms of the duplicate
			Duplicate& duplicate = seenIdMap.find(visibleIds[i])->second;

			unsigned int rotationType = 0;
			if (rotationTypes.length())
				rotationType = rotationTypes[i];
			else if (aimDirections.length())
				rotationType = 1;
			else if (aimPositions.length())
				rotationType = 2;

			MEulerRotation rotation;
			short rotationOrder = 0;
			if (rotationType == 0)
			{
				if (rotations.length())
					rotation = rotations[i];

				rotationOrder = fnDepInstancer.findPlug("rotationOrder", false).asShort();
				rotationOrder = rotationOrderMap.find(rotationOrder)->second;
				// We are assuming the input rotation is already ordered in the current order (ie. the x,y,z components will not change)
				// The output particle data which connects to the instancer should be using the same rotation order
				rotation.order = (MEulerRotation::RotationOrder)rotationOrder;
			}
			else
			{
				MVector aimDirection;
				if (rotationType == 1)
				{
					if (aimDirections.length())
						aimDirection = aimDirections[i];
					else
						aimDirection = { 1,0,0 };
				}
				else if (rotationType == 2)
				{
					if (aimPositions.length())
						aimDirection = aimPositions[i] - positions[i];
					else
						aimDirection = MVector{ 0,0,0 } - positions[i];
				}
				MVector aimAxis{ 1,0,0 };
				if (aimAxes.length())
					aimAxis = aimAxes[i];
				MVector aimWorldUpDirection{ 0,1,0 };
				if (aimWorldUpDirections.length())
					aimWorldUpDirection = aimWorldUpDirections[i];
				MVector aimUpAxis{ 0,1,0 };
				if (aimUpAxes.length())
					aimUpAxis = aimUpAxes[i];
				
				aimDirection.normalize();
				aimAxis.normalize();
				aimWorldUpDirection.normalize();
				aimUpAxis.normalize();

				aimDirectionXPlug.setDouble(aimDirection.x);
				aimDirectionYPlug.setDouble(aimDirection.y);
				aimDirectionZPlug.setDouble(aimDirection.z);
				aimAxisXPlug.setDouble(aimAxis.x);
				aimAxisYPlug.setDouble(aimAxis.y);
				aimAxisZPlug.setDouble(aimAxis.z);
				aimWorldUpDirectionXPlug.setDouble(aimWorldUpDirection.x);
				aimWorldUpDirectionYPlug.setDouble(aimWorldUpDirection.y);
				aimWorldUpDirectionZPlug.setDouble(aimWorldUpDirection.z);
				aimUpAxisXPlug.setDouble(aimUpAxis.x);
				aimUpAxisYPlug.setDouble(aimUpAxis.y);
				aimUpAxisZPlug.setDouble(aimUpAxis.z);

				rotation = MEulerRotation{ outRotateXPlug.asDouble(), outRotateYPlug.asDouble(), outRotateZPlug.asDouble() };
			}

			MVector scale{ 1,1,1 };
			if (scales.length())
				scale = scales[i];

			bool visibility = true;
			if (visibilities.length())
				visibility = visibilities[i];

			// The below transformations are pre-multiplied such that each sequential operation is applied relative to all previous operations
			// The multiplication order will be : s (initial) * r (initial) * t (initial) * s (particle) * r (particle) * t (particle)
			MTransformationMatrix compositionMatrix;
			// Apply particle transforms
			compositionMatrix.addTranslation(positions[i], MSpace::kPreTransform);
			compositionMatrix.rotateBy(rotation, MSpace::kPreTransform);
			compositionMatrix.addScale(&scale.x, MSpace::kPreTransform);
			// Apply initial transforms relative to particle transforms
			compositionMatrix.addTranslation(duplicate.initialPosition, MSpace::kPreTransform);
			compositionMatrix.rotateBy(duplicate.initialRotation, MSpace::kPreTransform);
			compositionMatrix.addScale(&duplicate.initialScale.x, MSpace::kPreTransform);

			MVector position = compositionMatrix.getTranslation(MSpace::kPostTransform);
			rotation = compositionMatrix.eulerRotation();
			rotation.reorderIt((MEulerRotation::RotationOrder)rotationOrder);
			compositionMatrix.getScale(&scale.x, MSpace::kPostTransform);

			// Set keyframes
			MFnAnimCurve fnAnimCurve;
			fnAnimCurve.setObject(duplicate.translateXAnimCurveObj);
			fnAnimCurve.addKey(iterTime, position.x, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.translateYAnimCurveObj);
			fnAnimCurve.addKey(iterTime, position.y, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.translateZAnimCurveObj);
			fnAnimCurve.addKey(iterTime, position.z, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.rotateXAnimCurveObj);
			fnAnimCurve.addKey(iterTime, rotation.x, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.rotateYAnimCurveObj);
			fnAnimCurve.addKey(iterTime, rotation.y, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.rotateZAnimCurveObj);
			fnAnimCurve.addKey(iterTime, rotation.z, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.scaleXAnimCurveObj);
			fnAnimCurve.addKey(iterTime, scale.x, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.scaleYAnimCurveObj);
			fnAnimCurve.addKey(iterTime, scale.y, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.scaleZAnimCurveObj);
			fnAnimCurve.addKey(iterTime, scale.z, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.rotateOrderAnimCurveObj);
			fnAnimCurve.addKey(iterTime, rotationOrder, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
			fnAnimCurve.setObject(duplicate.visibilityAnimCurveObj);
			fnAnimCurve.addKey(iterTime, visibility, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
		}

		// Hide all of the particles whose ids are contained in the hiddenIds set
		for (auto itHidden = hiddenIds.cbegin(); itHidden != hiddenIds.cend(); ++itHidden)
		{
			Duplicate& duplicate = seenIdMap.find(*itHidden)->second;
			MFnAnimCurve fnAnimCurve{ duplicate.visibilityAnimCurveObj };
			fnAnimCurve.addKey(iterTime, false, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &m_animChangeMod, &status);
		}

		iterTime += timeStep;
	}

	// Restore the previous time
	MAnimControl::setCurrentTime(currentTime);

	// Delete instancer related nodes if specified
	if (deleteSourceHierarchiesState)
	{
		for (auto itPath = inputHierarchyPathArray.begin(); itPath != inputHierarchyPathArray.end(); ++itPath)
			m_dagMod.deleteNode((*itPath).node());
	}
	if (deleteInstancerState)
		m_dagMod.deleteNode(instancerPath.node());

	// Cleanup
	m_dagMod.deleteNode(aimConstraintObj);
	m_dagMod.doIt();

	return status;
}

// Creates animCurve nodes and sets keyframes
MStatus InstancerBake::redoIt()
{
	MStatus status;

	status = m_dagMod.doIt();
	if (status)
	{
		status = m_animChangeMod.redoIt();
	}

	return status;
}

// Deletes keyframes then deletes animCurve nodes
MStatus InstancerBake::undoIt()
{
	MStatus status;

	status = m_animChangeMod.undoIt();
	if (status)
	{
		status = m_dagMod.undoIt();
	}

	return status;
}

// Cleanup
#undef kErrorFlagNotSet
#undef kErrorParsingFlag
#undef kErrorInvalidTimeStep
#undef kErrorInvalidTimeInterval
#undef kErrorNoValidObject
#undef kErrorInvalidInstancer
#undef kErrorInvalidParent
#undef kWarningInvalidConnection
#undef kErrorParticleDataRequired
#undef kErrorParticleDataIncorrectSize
#undef kErrorInvalidParticleObjectIndex