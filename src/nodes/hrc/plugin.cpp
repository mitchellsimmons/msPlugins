#include <maya/MFnPlugin.h>

#include "hrc_matrix.h"
#include "hrc.h"
#include "module_hrc.h"
#include "rig_hrc.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Project Block0 IDs : [0x001310f0 - 0x001310ff]
const MTypeId HrcMatrix::kTypeId = 0x001310f0;
const MTypeId Hrc::kTypeId = 0x001310f1;
const MTypeId ModuleHrc::kTypeId = 0x001310f2;
const MTypeId RigHrc::kTypeId = 0x001310f3;

// Names
const MString Hrc::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Hrc";
const MString ModuleHrc::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ModuleHrc";
const MString RigHrc::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "RigHrc";

// ------ Exports -------------------------------------------------------------------------------------------------------------------------------------------------------------

#define kErrorInvalidPluginId \
	"Plugin \"^1s\" has been given an invalid id"

#define kErrorPluginRegistration \
	"Error registering plugin \"^1s\""

#define kErrorPluginDeregistration \
	"Error deregistering plugin \"^1s\""

MStatus initializePlugin(MObject plugin)
{
	MStatus status;
	MString errorMessage;

	MFnPlugin fnPlugin(plugin, "Mitchell Simmons", MRS_XSTR(PROJECT_VERSION), "Any");
	fnPlugin.setName(MRS_XSTR(TARGET_NAME));

	// |------------|
	// |  Validate  |
	// |------------|

	errorMessage.format(kErrorInvalidPluginId, MString("Hrc->HrcMatrix"));
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(HrcMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, Hrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Hrc::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ModuleHrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ModuleHrc::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, RigHrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(RigHrc::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// |------------|
	// |  Register  |
	// |------------|

	errorMessage.format(kErrorPluginRegistration, Hrc::kTypeName);
	status = MRS::registerTransform<HrcMatrix, Hrc>(fnPlugin);
	MRS_CHECK_ERROR_RETURN_MSTATUS(status, errorMessage);

	errorMessage.format(kErrorPluginRegistration, ModuleHrc::kTypeName);
	status = MRS::registerTransform<HrcMatrix, ModuleHrc>(fnPlugin);
	MRS_CHECK_ERROR_RETURN_MSTATUS(status, errorMessage);

	errorMessage.format(kErrorPluginRegistration, RigHrc::kTypeName);
	status = MRS::registerTransform<HrcMatrix, RigHrc>(fnPlugin);
	MRS_CHECK_ERROR_RETURN_MSTATUS(status, errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);

	// |--------------|
	// |  Deregister  |
	// |--------------|

	errorMessage.format(kErrorPluginDeregistration, Hrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Hrc>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ModuleHrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ModuleHrc>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, RigHrc::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<RigHrc>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}