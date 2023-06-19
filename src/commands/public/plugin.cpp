#include <maya/MFnPlugin.h>

#include "instancerBake_cmd.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

const MString InstancerBake::kCommandName = "instancerBake";

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
	errorMessage.format(kErrorPluginRegistration, InstancerBake::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<InstancerBake>(pluginFn, true /* syntax */), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject pluginObj)
{
	MString errorMessage;

	MFnPlugin pluginFn(pluginObj);

	// Deregister
	errorMessage.format(kErrorPluginDeregistration, InstancerBake::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<InstancerBake>(pluginFn), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
