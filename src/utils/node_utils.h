#pragma once

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

#include <maya/MAngle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MDataBlock.h>
#include <maya/MDistance.h>
#include <maya/MDoubleArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVector.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MIntArray.h>
#include <maya/MMatrix.h>
#include <maya/MMatrixArray.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MQuaternion.h>
#include <maya/MRampAttribute.h>
#include <maya/MString.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>

#include <SeExpr2/Curve.h>
#include <SeExpr2/Vec.h>

#include "data_utils.h"
#include "math_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class NodeHelper
{
public:
	// Encode multiple flags from the bit field or use one of the below presets (or a combination of both)
	enum AttributeFlags : int32_t
	{
		kReadable = 1 << 0,
		kWritable = 1 << 1,
		kConnectable = 1 << 2,
		kStorable = 1 << 3,
		kCached = 1 << 4,
		kArray = 1 << 5,
		kIndexMatters = 1 << 6,					
		kKeyable = 1 << 7,
		kChannelBox = 1 << 8,
		kHidden = 1 << 9,
		kUsedAsColor = 1 << 10,
		kIndeterminant = 1 << 11,
		kRenderSource = 1 << 12,
		kWorldSpace = 1 << 13,
		kAffectsWorldSpace = 1 << 14,
		kUsedAsFileName = 1 << 15,
		kAffectsAppearance = 1 << 16,
		kProxyAttribute = 1 << 17,
		kDisconnectBehaviorDelete = 1 << 18,
		kDisconnectBehaviorReset = 1 << 19,
		kDisconnectBehaviorNothing = 1 << 20,
		kUsesArrayDataBuilder = 1 << 21,
		kInternal = 1 << 22,
	};

	enum AttributePresets : int32_t
	{
		kDefaultPreset =		kReadable | kWritable | kConnectable | kStorable | kCached, // Won't show in channelBox
		kReadOnlyPreset =		kReadable | kConnectable | kCached, // Source plug only (data not saved with scene)
		kWriteOnlyPreset =		kWritable | kConnectable | kStorable | kCached, // Destination plug only
		kArrayPreset =			kReadable | kWritable | kConnectable | kStorable | kCached | kArray | kUsesArrayDataBuilder | kDisconnectBehaviorNothing,
		kReadOnlyArrayPreset =	kReadable | kConnectable | kCached | kArray | kUsesArrayDataBuilder | kDisconnectBehaviorNothing,
		kWriteOnlyArrayPreset =	kWritable | kConnectable | kStorable | kCached | kArray | kUsesArrayDataBuilder | kIndexMatters | kDisconnectBehaviorNothing // Index matters for non-readable element plugs
	};

	// ------ Create ----------------------------------------------------------------------------------------------------------------------------------------------------------

	// ------ MFnNumericAttribute ------
	static void createBoolAttribute(MObject& outAttr, const char* longName, const char* shortName, bool value, int32_t flags);
	static void createIntAttribute(MObject& outAttr, const char* longName, const char* shortName, int value, int32_t flags);
	static void createFloatAttribute(MObject& outAttr, const char* longName, const char* shortName, float value, int32_t flags);
	static void createDoubleAttribute(MObject& outAttr, const char* longName, const char* shortName, double value, int32_t flags);
	static void createFloatVectorAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
		const char* longName, const char* shortName, const MFloatVector& value, int32_t flags); // MFnNumericData::k3Float
	static void createVectorAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
		const char* longName, const char* shortName, const MVector& value, int32_t flags); // MFnNumericData::k3Double

	// ------ MFnUnitAttribute ------
	static void createAngleAttribute(MObject& outAttr, const char* longName, const char* shortName, const MAngle& value, int32_t flags);
	static void createDistanceAttribute(MObject& outAttr, const char* longName, const char* shortName, const MDistance& value, int32_t flags);
	static void createTimeAttribute(MObject& outAttr, const char* longName, const char* shortName, const MTime& value, int32_t flags);

	// ------ MFnMatrixAttribute ------
	static void createFloatMatrixAttribute(MObject& outAttr, const char* longName, const char* shortName, const MFloatMatrix& value, int32_t flags);
	static void createMatrixAttribute(MObject& outAttr, const char* longName, const char* shortName, const MMatrix& value, int32_t flags);

	// ------ MFnMessageAttribute ------
	static void createMessageAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);

	// ------ MFnEnumAttribute ------
	static void createEasingAttribute(MObject& outAttr, const char* longName, const char* shortName, MRS::Easing defaultValue, int32_t flags);
	static void createEnumAttribute(MObject& outAttr, const char* longName, const char* shortName, std::unordered_map<const char*, short>& value, short defaultValue, int32_t flags);

	// ------ MFnCompoundAttribute ------
	static void createQuaternionAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr, MObject& outChildWAttr,
		const char* longName, const char* shortName, const MQuaternion& value, int32_t flags);
	static void createEulerAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
		const char* longName, const char* shortName, const MEulerRotation& value, int32_t flags);
	static void createCompoundAttribute(MObject& outParentAttr, const std::vector<MObject>& children, const char* longName, const char* shortName, int32_t flags);

	// ------ MFnTypedAttribute ------
	static void createIntDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<int>& value, int32_t flags);
	static void createFloatDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<float>& value, int32_t flags);
	static void createDoubleDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<double>& value, int32_t flags);
	static void createVectorDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MVector>& value, int32_t flags);
	static void createPointDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MPoint>& value, int32_t flags);
	static void createMatrixDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MMatrix>& value, int32_t flags);
	static void createNurbsCurveAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);
	static void createNurbsSurfaceAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);
	static void createMeshAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);

	// Custom data plugin
	template<typename TDataPlugin, typename TData>
	static void createPluginDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<TData>& value, int32_t flags)
	{
		MFnPluginData fnData;
		MFnTypedAttribute fnAttr;

		MObject dataObj = fnData.create(TDataPlugin::kTypeId);
		TDataPlugin* customData = (TDataPlugin*)fnData.data();
		customData->setArray(value);

		outAttr = fnAttr.create(longName, shortName, TDataPlugin::kTypeId, dataObj);
		setAttributeFlags(fnAttr, flags);
	}

	// ------ MRampAttribute ------
	static void createCurveRampAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);
	static void createColorRampAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags);

	// ------ Get ------------------------------------------------------------------------------------------------------------------------------------------------------

	// ------ MFnNumericAttribute ------
	static bool inputBoolValue(MDataBlock& dataBlock, const MObject& attr);
	static int inputIntValue(MDataBlock& dataBlock, const MObject& attr);
	static float inputFloatValue(MDataBlock& dataBlock, const MObject& attr);
	static double inputDoubleValue(MDataBlock& dataBlock, const MObject& attr);
	static MFloatVector inputFloatVectorValue(MDataBlock& dataBlock, const MObject& attr); // MFnNumericData::k3Float
	static MVector inputVectorValue(MDataBlock& dataBlock, const MObject& attr); // MFnNumericData::k3Double

	static std::vector<bool> inputBoolArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<int> inputIntArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<float> inputFloatArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<double> inputDoubleArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MFloatVector> inputFloatVectorArrayValue(MDataBlock& dataBlock, const MObject& attr); // MFnNumericData::k3Float
	static std::vector<MVector> inputVectorArrayValue(MDataBlock& dataBlock, const MObject& attr); // MFnNumericData::k3Double

	// ------ MFnUnitAttribute ------
	static MAngle inputAngleValue(MDataBlock& dataBlock, const MObject& attr);
	static MDistance inputDistanceValue(MDataBlock& dataBlock, const MObject& attr);
	static MTime inputTimeValue(MDataBlock& dataBlock, const MObject& attr);

	static std::vector<MAngle> inputAngleArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MDistance> inputDistanceArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MTime> inputTimeArrayValue(MDataBlock& dataBlock, const MObject& attr);

	// ------ MFnMatrixAttribute ------
	static MFloatMatrix inputFloatMatrixValue(MDataBlock& dataBlock, const MObject& attr);
	static MMatrix inputMatrixValue(MDataBlock& dataBlock, const MObject& attr);

	static std::vector<MFloatMatrix> inputFloatMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MMatrix> inputMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr);

	// ------ MFnEnumAttribute ------
	static MRS::Easing inputEasingValue(MDataBlock& dataBlock, const MObject& attr);
	static short inputEnumValue(MDataBlock& dataBlock, const MObject& attr);

	static std::vector<MRS::Easing> inputEasingArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<short> inputEnumArrayValue(MDataBlock& dataBlock, const MObject& attr);

	// ------ MFnCompoundAttribute ------
	static std::vector<bool> inputBoolCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children);
	static std::vector<int> inputIntCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children);
	static std::vector<float> inputFloatCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children);
	static std::vector<double> inputDoubleCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children);
	static MQuaternion inputQuaternionValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr, const MObject childWAttr);
	static MEulerRotation inputEulerValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr);

	// Unlikely to need an array of numeric compounds..
	
	static std::vector<MQuaternion> inputQuaternionArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr, const MObject childWAttr);
	static std::vector<MEulerRotation> inputEulerArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr);

	// ------ MFnTypedAttribute ------
	static std::vector<int> inputIntDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<float> inputFloatDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<double> inputDoubleDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MVector> inputVectorDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MPoint> inputPointDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static std::vector<MMatrix> inputMatrixDataArrayValue(MDataBlock& dataBlock, const MObject& attr);
	static MObject inputNurbsCurveValue(MDataBlock& dataBlock, const MObject& attr);
	static MObject inputNurbsSurfaceValue(MDataBlock& dataBlock, const MObject& attr);
	static MObject inputMeshValue(MDataBlock& dataBlock, const MObject& attr);

	// Unlikely to need an array of data arrays..

	// Custom data plugin
	template<typename TDataPlugin, typename TData>
	static std::vector<TData> inputPluginDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
	{
		std::vector<TData> out;
		MDataHandle handle = dataBlock.inputValue(attr);
		MObject dataObj = handle.data();
		MFnPluginData fnData(dataObj);
		TDataPlugin* customData = (TDataPlugin*)fnData.data();
		customData->getArray(out);
		
		return out;
	}

	// ------ MRampAttribute ------
	static SeExpr2::Curve<double> inputCurveRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, 
		MObject& valueAttr, MObject& interpAttr);
	static SeExpr2::Curve<SeExpr2::Vec3d> inputColorRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, 
		MObject& colorAttr, MObject& interpAttr);

	// ------ Set ------------------------------------------------------------------------------------------------------------------------------------------------------

	// ------ MFnNumericAttribute ------
	static void outputBoolValue(MDataBlock& dataBlock, const MObject& attr, const bool value);
	static void outputIntValue(MDataBlock& dataBlock, const MObject& attr, const int value);
	static void outputFloatValue(MDataBlock& dataBlock, const MObject& attr, const float value);
	static void outputDoubleValue(MDataBlock& dataBlock, const MObject& attr, const double value);
	static void outputFloatVectorValue(MDataBlock& dataBlock, const MObject& attr, const MFloatVector& value); // MFnNumericData::k3Float
	static void outputVectorValue(MDataBlock& dataBlock, const MObject& attr, const MVector& value); // MFnNumericData::k3Double

	static void outputBoolArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<bool>& values);
	static void outputIntArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<int>& values);
	static void outputFloatArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<float>& values);
	static void outputDoubleArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<double>& values);
	static void outputFloatVectorArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MFloatVector>& values); // MFnNumericData::k3Float
	static void outputVectorArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MVector>& values); // MFnNumericData::k3Double

	// ------ MFnUnitAttribute ------
	static void outputAngleValue(MDataBlock& dataBlock, const MObject& attr, const MAngle& value);
	static void outputDistanceValue(MDataBlock& dataBlock, const MObject& attr, const MDistance& value);
	static void outputTimeValue(MDataBlock& dataBlock, const MObject& attr, const MTime& value);

	static void outputAngleArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MAngle>& values);
	static void outputDistanceArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MDistance>& values);
	static void outputTimeArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MTime>& values);

	// ------ MFnMatrixAttribute ------
	static void outputFloatMatrixValue(MDataBlock& dataBlock, const MObject& attr, const MFloatMatrix& value);
	static void outputMatrixValue(MDataBlock& dataBlock, const MObject& attr, const MMatrix& value);

	static void outputFloatMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MFloatMatrix>& values);
	static void outputMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MMatrix>& values);

	// ------ MFnEnumAttribute ------
	static void outputEasingValue(MDataBlock& dataBlock, const MObject& attr, const MRS::Easing value);
	static void outputEnumValue(MDataBlock& dataBlock, const MObject& attr, const short value);

	static void outputEasingArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MRS::Easing>& value);
	static void outputEnumArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<short>& value);

	// ------ MFnCompoundAttribute ------
	static void outputBoolCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<bool>& value);
	static void outputIntCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<int>& value);
	static void outputFloatCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<float>& value);
	static void outputDoubleCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<double>& value);
	static void outputQuaternionValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr, const MObject childWAttr, const MQuaternion& value);
	static void outputEulerValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
		const MObject childZAttr, const MEulerRotation& value);

	// Unlikely to need an array of numeric compounds..

	static void outputQuaternionArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject& childXAttr, const MObject& childYAttr,
		const MObject& childZAttr, const MObject& childWAttr, const std::vector<MQuaternion>& values);
	static void outputEulerArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject& childXAttr, const MObject& childYAttr,
		const MObject& childZAttr, const std::vector<MEulerRotation>& values);

	// ------ MFnTypedAttribute ------
	static void outputIntDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<int>& values);
	static void outputFloatDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<float>& values);
	static void outputDoubleDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<double>& values);
	static void outputVectorDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MVector>& values);
	static void outputPointDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MPoint>& values);
	static void outputMatrixDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MMatrix>& values);

	// Unlikely to need an array of data arrays..

	// Custom data plugin
	template<typename TDataPlugin, typename TData>
	static void outputPluginDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<TData>& values)
	{
		MDataHandle outHandle = dataBlock.outputValue(attr);
		MObject dataObj = outHandle.data();
		MFnPluginData fnData(dataObj);
		TDataPlugin* customData = (TDataPlugin*)fnData.data();
		customData->setArray(values);
		outHandle.setMPxData((MPxData*)customData);
		outHandle.setClean();
	}

	// ------ MRampAttribute ------
	static void outputCurveRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, MObject& valueAttr, MObject& interpAttr,
		std::vector<SeExpr2::Curve<double>::CV>& values);
	static void outputColorRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, MObject& colorAttr, MObject& interpAttr,
		std::vector<SeExpr2::Curve<SeExpr2::Vec3d>::CV>& values);

	// ------ Utilities ------
	template<typename TType>
	static void setMin(MObject obj, TType min)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMin(min);
	}

	template<typename TType>
	static void setMin(MObject obj, TType minimum, TType softmin)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setSoftMin(softmin);
	}

	template<>
	static void setMin(MObject obj, MAngle min)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
	}

	template<>
	static void setMin(MObject obj, MAngle minimum, MAngle softmin)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setSoftMin(softmin);
	}

	template<>
	static void setMin(MObject obj, MDistance min)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
	}

	template<>
	static void setMin(MObject obj, MDistance minimum, MDistance softmin)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setSoftMin(softmin);
	}

	template<>
	static void setMin(MObject obj, MTime min)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
	}

	template<>
	static void setMin(MObject obj, MTime minimum, MTime softmin)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setSoftMin(softmin);
	}

	template<typename TType>
	static void setMax(MObject obj, TType max)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMax(max);
	}

	template<typename TType>
	static void setMax(MObject obj, TType maximum, TType softmax)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMax(MObject obj, MAngle max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(max);
	}

	template<>
	static void setMax(MObject obj, MAngle maximum, MAngle softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMax(MObject obj, MDistance max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(max);
	}

	template<>
	static void setMax(MObject obj, MDistance maximum, MDistance softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMax(MObject obj, MTime max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(max);
	}

	template<>
	static void setMax(MObject obj, MTime maximum, MTime softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMax(softmax);
	}

	template<typename TType>
	static void setMinMax(MObject obj, TType min, TType max)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMin(min);
		fnAttr.setMax(max);
	}

	template<typename TType>
	static void setMinMax(MObject obj, TType minimum, TType maximum, TType softmin, TType softmax)
	{
		MFnNumericAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMin(softmin);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMinMax(MObject obj, MAngle min, MAngle max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
		fnAttr.setMax(max);
	}

	template<>
	static void setMinMax(MObject obj, MAngle minimum, MAngle maximum, MAngle softmin, MAngle softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMin(softmin);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMinMax(MObject obj, MDistance min, MDistance max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
		fnAttr.setMax(max);
	}

	template<>
	static void setMinMax(MObject obj, MDistance minimum, MDistance maximum, MDistance softmin, MDistance softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMin(softmin);
		fnAttr.setSoftMax(softmax);
	}

	template<>
	static void setMinMax(MObject obj, MTime min, MTime max)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(min);
		fnAttr.setMax(max);
	}

	template<>
	static void setMinMax(MObject obj, MTime minimum, MTime maximum, MTime softmin, MTime softmax)
	{
		MFnUnitAttribute fnAttr(obj);
		fnAttr.setMin(minimum);
		fnAttr.setMax(maximum);
		fnAttr.setSoftMin(softmin);
		fnAttr.setSoftMax(softmax);
	}

	static void setNiceName(MObject& attr, const char* niceName);

private:
	// ------ Helpers ------
	static void setAttributeFlags(MFnAttribute& fnAttr, int32_t flags);
	// Filters out flags which are irrelevant to child attributes (eg. for a compound array attribute we do not want children to also be arrays)
	// This should only be used for child attributes created as part of a function call, not ones passed in (ie. not for MFnCompoundAttribute children)
	static void setChildAttributeFlags(MFnAttribute& fnAttr, int32_t flags);
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------