#pragma once

#include <maya/MFnPlugin.h>

#include "angleArray_data.h"
#include "eulerArray_data.h"
#include "quaternionArray_data.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Exports -------------------------------------------------------------------------------------------------------------------------------------------------------------

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
	errorMessage.format(kErrorInvalidPluginId, AngleArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AngleArrayData::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, EulerArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(EulerArrayData::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, QuaternionArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(QuaternionArrayData::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// Register
	errorMessage.format(kErrorPluginRegistration, AngleArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerData<AngleArrayData>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, EulerArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerData<EulerArrayData>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, QuaternionArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerData<QuaternionArrayData>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);
	
	// Deregister
	errorMessage.format(kErrorPluginDeregistration, AngleArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterData<AngleArrayData>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, EulerArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterData<EulerArrayData>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, QuaternionArrayData::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterData<QuaternionArrayData>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------