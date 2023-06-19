#include <maya/MFnPlugin.h>

#include "matrix/packMatrixArray_node.h"
#include "matrix/unpackMatrixArray_node.h"
#include "matrix/composeMatrixArray_node.h"
#include "matrix/decomposeMatrixArray_node.h"
#include "vector/packVectorArray_node.h"
#include "vector/unpackVectorArray_node.h"
#include "vector/arithmeticSequenceVectorArray_node.h"
#include "vector/geometricSequenceVectorArray_node.h"
#include "euler/packEulerArray_node.h"
#include "euler/unpackEulerArray_node.h"
#include "euler/arithmeticSequenceEulerArray_node.h"
#include "euler/geometricSequenceEulerArray_node.h"
#include "quaternion/packQuaternionArray_node.h"
#include "quaternion/unpackQuaternionArray_node.h"
#include "angle/packAngleArray_node.h"
#include "angle/unpackAngleArray_node.h"
#include "angle/arithmeticSequenceAngleArray_node.h"
#include "angle/geometricSequenceAngleArray_node.h"
#include "double/packDoubleArray_node.h"
#include "double/unpackDoubleArray_node.h"
#include "double/arithmeticSequenceDoubleArray_node.h"
#include "double/geometricSequenceDoubleArray_node.h"
#include "int/packIntArray_node.h"
#include "int/unpackIntArray_node.h"
#include "int/arithmeticSequenceIntArray_node.h"
#include "int/geometricSequenceIntArray_node.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Project Block0 IDs : [0x00130fd0 - 0x00130fdf]
const MTypeId PackMatrixArray::kTypeId = 0x00130fd0;
const MTypeId UnpackMatrixArray::kTypeId = 0x00130fd1;
const MTypeId PackVectorArray::kTypeId = 0x00130fd2;
const MTypeId UnpackVectorArray::kTypeId = 0x00130fd3;
const MTypeId PackEulerArray::kTypeId = 0x00130fd4;
const MTypeId UnpackEulerArray::kTypeId = 0x00130fd5;
const MTypeId PackQuaternionArray::kTypeId = 0x00130fd6;
const MTypeId UnpackQuaternionArray::kTypeId = 0x00130fd7;
const MTypeId PackAngleArray::kTypeId = 0x00130fd8;
const MTypeId UnpackAngleArray::kTypeId = 0x00130fd9;
const MTypeId PackDoubleArray::kTypeId = 0x00130fda;
const MTypeId UnpackDoubleArray::kTypeId = 0x00130fdb;
const MTypeId PackIntArray::kTypeId = 0x00130fdc;
const MTypeId UnpackIntArray::kTypeId = 0x00130fdd;
const MTypeId ComposeMatrixArray::kTypeId = 0x00130fde;
const MTypeId DecomposeMatrixArray::kTypeId = 0x00130fdf;
// Project Block1 IDs : [0x00131030 - 0x0013103f]
const MTypeId ArithmeticSequenceVectorArray::kTypeId = 0x00131030;
const MTypeId GeometricSequenceVectorArray::kTypeId = 0x00131031;
const MTypeId ArithmeticSequenceEulerArray::kTypeId = 0x00131032;
const MTypeId GeometricSequenceEulerArray::kTypeId = 0x00131033;
const MTypeId ArithmeticSequenceAngleArray::kTypeId = 0x00131034;
const MTypeId GeometricSequenceAngleArray::kTypeId = 0x00131035;
const MTypeId ArithmeticSequenceDoubleArray::kTypeId = 0x00131036;
const MTypeId GeometricSequenceDoubleArray::kTypeId = 0x00131037;
const MTypeId ArithmeticSequenceIntArray::kTypeId = 0x00131038;
const MTypeId GeometricSequenceIntArray::kTypeId = 0x00131039;


// Names
const MString PackMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackMatrixArray";
const MString UnpackMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackMatrixArray";
const MString PackVectorArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackVectorArray";
const MString UnpackVectorArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackVectorArray";
const MString PackEulerArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackEulerArray";
const MString UnpackEulerArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackEulerArray";
const MString PackQuaternionArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackQuaternionArray";
const MString UnpackQuaternionArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackQuaternionArray";
const MString PackAngleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackAngleArray";
const MString UnpackAngleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackAngleArray";
const MString PackDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackDoubleArray";
const MString UnpackDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackDoubleArray";
const MString PackIntArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "PackIntArray";
const MString UnpackIntArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "UnpackIntArray";
const MString ComposeMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ComposeMatrixArray";
const MString DecomposeMatrixArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "DecomposeMatrixArray";
const MString ArithmeticSequenceVectorArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ArithmeticSequenceVectorArray";
const MString GeometricSequenceVectorArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "GeometricSequenceVectorArray";
const MString ArithmeticSequenceEulerArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ArithmeticSequenceEulerArray";
const MString GeometricSequenceEulerArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "GeometricSequenceEulerArray";
const MString ArithmeticSequenceAngleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ArithmeticSequenceAngleArray";
const MString GeometricSequenceAngleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "GeometricSequenceAngleArray";
const MString ArithmeticSequenceDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ArithmeticSequenceDoubleArray";
const MString GeometricSequenceDoubleArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "GeometricSequenceDoubleArray";
const MString ArithmeticSequenceIntArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "ArithmeticSequenceIntArray";
const MString GeometricSequenceIntArray::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "GeometricSequenceIntArray";

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
	errorMessage.format(kErrorInvalidPluginId, PackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackVectorArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackVectorArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackEulerArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackEulerArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackQuaternionArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackQuaternionArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackAngleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackAngleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, PackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(PackIntArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, UnpackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(UnpackIntArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ComposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ComposeMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, DecomposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(DecomposeMatrixArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ArithmeticSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ArithmeticSequenceVectorArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, GeometricSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(GeometricSequenceVectorArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ArithmeticSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ArithmeticSequenceEulerArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, GeometricSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(GeometricSequenceEulerArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ArithmeticSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ArithmeticSequenceAngleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, GeometricSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(GeometricSequenceAngleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ArithmeticSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ArithmeticSequenceDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, GeometricSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(GeometricSequenceDoubleArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, ArithmeticSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(ArithmeticSequenceIntArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, GeometricSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(GeometricSequenceIntArray::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// Register
	errorMessage.format(kErrorPluginRegistration, PackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, PackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<PackIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, UnpackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<UnpackIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ComposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ComposeMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, DecomposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<DecomposeMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ArithmeticSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ArithmeticSequenceVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, GeometricSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<GeometricSequenceVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ArithmeticSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ArithmeticSequenceEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, GeometricSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<GeometricSequenceEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ArithmeticSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ArithmeticSequenceAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, GeometricSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<GeometricSequenceAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ArithmeticSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ArithmeticSequenceDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, GeometricSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<GeometricSequenceDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, ArithmeticSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<ArithmeticSequenceIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, GeometricSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<GeometricSequenceIntArray>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);

	// Deregister
	errorMessage.format(kErrorPluginDeregistration, PackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackQuaternionArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackQuaternionArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, PackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<PackIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, UnpackIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<UnpackIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ComposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ComposeMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, DecomposeMatrixArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<DecomposeMatrixArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ArithmeticSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ArithmeticSequenceVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, GeometricSequenceVectorArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<GeometricSequenceVectorArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ArithmeticSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ArithmeticSequenceEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, GeometricSequenceEulerArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<GeometricSequenceEulerArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ArithmeticSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ArithmeticSequenceAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, GeometricSequenceAngleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<GeometricSequenceAngleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ArithmeticSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ArithmeticSequenceDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, GeometricSequenceDoubleArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<GeometricSequenceDoubleArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, ArithmeticSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<ArithmeticSequenceIntArray>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, GeometricSequenceIntArray::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<GeometricSequenceIntArray>(fnPlugin), errorMessage);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------