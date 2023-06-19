#include <maya/MFnPlugin.h>

#include "nodeBoxContainer_node.h"
#include "stickyContainer_node.h"
#include "nodeEditorTabInfoMetadata_cmd.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Project Block0 IDs : [0x00131020 - 0x0013102f]
const MTypeId NodeBoxContainer::kTypeId = 0x00131020;
const MTypeId StickyContainer::kTypeId = 0x00131021;

// Names (prefix is baked for private nodes/commands)
const MString NodeBoxContainer::kTypeName = "mrs_NodeBoxContainer";
const MString StickyContainer::kTypeName = "mrs_StickyContainer";
const MString NodeBoxCommand::kCommandName = "mrs_NodeBox";
const MString StickyCommand::kCommandName = "mrs_Sticky";

// ------ Exports -----------------------------------------------------------------------------------------------------------------------------------------------------------

#define kErrorInvalidPluginId \
	"Plugin \"^1s\" has been given an invalid id"

#define kErrorPluginRegistration \
	"Error registering plugin \"^1s\""

#define kErrorPluginDeregistration \
	"Error deregistering plugin \"^1s\""

MStatus initializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin, "Mitchell Simmons", MRS_XSTR(PROJECT_VERSION), "Any");
	fnPlugin.setName(MRS_XSTR(TARGET_NAME));

	// Validate
	errorMessage.format(kErrorInvalidPluginId, NodeBoxContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(NodeBoxContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);
	errorMessage.format(kErrorInvalidPluginId, StickyContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(StickyContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// Register Nodes
	errorMessage.format(kErrorPluginRegistration, NodeBoxContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<NodeBoxContainer>(fnPlugin), errorMessage);
	errorMessage.format(kErrorPluginRegistration, StickyContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<StickyContainer>(fnPlugin), errorMessage);

	// Register Commands
	errorMessage.format(kErrorPluginRegistration, NodeBoxCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<NodeBoxCommand>(fnPlugin), errorMessage);
	errorMessage.format(kErrorPluginRegistration, StickyCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<StickyCommand>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);

	// Deregister Nodes
	errorMessage.format(kErrorPluginDeregistration, NodeBoxContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<NodeBoxContainer>(fnPlugin), errorMessage);
	errorMessage.format(kErrorPluginDeregistration, StickyContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<StickyContainer>(fnPlugin), errorMessage);

	// Deregister Commands
	errorMessage.format(kErrorPluginDeregistration, NodeBoxCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<NodeBoxCommand>(fnPlugin), errorMessage);
	errorMessage.format(kErrorPluginDeregistration, StickyCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<StickyCommand>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------