#include <maya/MFnPlugin.h>

#include "undoModifier_cmd.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

const MString UndoModifier::kCommandName = "undoModifier";

// ------ Exports -------------------------------------------------------------------------------------------------------------------------------------------------------------

#define kErrorPluginRegistration \
	"Error registering plugin \"^1s\""

#define kErrorPluginDeregistration \
	"Error deregistering plugin \"^1s\""

MStatus initializePlugin(MObject pluginObj)
{
	MString errorMessage;

	MFnPlugin pluginFn(pluginObj, "Mitchell Simmons", MRS_XSTR(PROJECT_VERSION), "Any");
	pluginFn.setName(MRS_XSTR(TARGET_NAME));

	// Register
	errorMessage.format(kErrorPluginRegistration, UndoModifier::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<UndoModifier>(pluginFn, true /* syntax */), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject pluginObj)
{
	MString errorMessage;

	MFnPlugin pluginFn(pluginObj);

	// Deregister
	errorMessage.format(kErrorPluginDeregistration, UndoModifier::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<UndoModifier>(pluginFn), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
