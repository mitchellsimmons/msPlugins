#include "node_utils.h"

/*	Notes
	-----
	For array attributes, use a constructor for creating a data builder
	- MArrayDataBuilder::MArrayDataBuilder(MDataBlock* block, const MObject& attribute, unsigned int numElements)
	This should be favored over MArrayDataHandle::builder() as the former allows the array to be resized to the current element count
	Whilst the latter will only allow the array to expand from its current state unless the following explicit call is made to reduce the size
	- MArrayDataBuilder::removeElement(unsigned index)    */

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ------ Create --------------------------------------------------------------------------------------------------------------------------------------------------

// ------ MFnNumericAttribute ------
void NodeHelper::createBoolAttribute(MObject& outAttr, const char* longName, const char* shortName, bool value, int32_t flags)
{
	MFnNumericAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnNumericData::kBoolean, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createIntAttribute(MObject& outAttr, const char* longName, const char* shortName, int value, int32_t flags)
{
	MFnNumericAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnNumericData::kInt, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createFloatAttribute(MObject& outAttr, const char* longName, const char* shortName, float value, int32_t flags)
{
	MFnNumericAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnNumericData::kFloat, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createDoubleAttribute(MObject& outAttr, const char* longName, const char* shortName, double value, int32_t flags)
{
	MFnNumericAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnNumericData::kDouble, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createFloatVectorAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
	const char* longName, const char* shortName, const MFloatVector& value, int32_t flags)
{
	MFnNumericAttribute fnAttr;

	const std::string attrXShortName = (std::string(shortName) + "X");
	const std::string attrXLongName = (std::string(longName) + "X");
	outChildXAttr = fnAttr.create(attrXShortName.c_str(), attrXLongName.c_str(), MFnNumericData::kFloat, value.x);
	setChildAttributeFlags(fnAttr, flags);

	const std::string attrYShortName = (std::string(shortName) + "Y");
	const std::string attrYLongName = (std::string(longName) + "Y");
	outChildYAttr = fnAttr.create(attrYShortName.c_str(), attrYLongName.c_str(), MFnNumericData::kFloat, value.y);
	setChildAttributeFlags(fnAttr, flags);

	const std::string attrZShortName = (std::string(shortName) + "Z");
	const std::string attrZLongName = (std::string(longName) + "Z");
	outChildZAttr = fnAttr.create(attrZShortName.c_str(), attrZLongName.c_str(), MFnNumericData::kFloat, value.z);
	setChildAttributeFlags(fnAttr, flags);

	outParentAttr = fnAttr.create(longName, shortName, outChildXAttr, outChildYAttr, outChildZAttr);
	setAttributeFlags(fnAttr, flags);
}

// Note, if adding this attribute to a compound, edit boxes will not be visible in the attribute editor
// This issue is not present when using floats
void NodeHelper::createVectorAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
	const char* longName, const char* shortName, const MVector& value, int32_t flags)
{
	MFnNumericAttribute fnAttr;

	const std::string attrXShortName = (std::string(shortName) + "X");
	const std::string attrXLongName = (std::string(longName) + "X");
	outChildXAttr = fnAttr.create(attrXShortName.c_str(), attrXLongName.c_str(), MFnNumericData::kDouble, value.x);
	setChildAttributeFlags(fnAttr, flags);

	const std::string attrYShortName = (std::string(shortName) + "Y");
	const std::string attrYLongName = (std::string(longName) + "Y");
	outChildYAttr = fnAttr.create(attrYShortName.c_str(), attrYLongName.c_str(), MFnNumericData::kDouble, value.y);
	setChildAttributeFlags(fnAttr, flags);

	const std::string attrZShortName = (std::string(shortName) + "Z");
	const std::string attrZLongName = (std::string(longName) + "Z");
	outChildZAttr = fnAttr.create(attrZShortName.c_str(), attrZLongName.c_str(), MFnNumericData::kDouble, value.z);
	setChildAttributeFlags(fnAttr, flags);

	outParentAttr = fnAttr.create(longName, shortName, outChildXAttr, outChildYAttr, outChildZAttr);
	setAttributeFlags(fnAttr, flags);
}

// ------ MFnUnitAttribute ------
void NodeHelper::createAngleAttribute(MObject& outAttr, const char* longName, const char* shortName, const MAngle& value, int32_t flags)
{
	MFnUnitAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createDistanceAttribute(MObject& outAttr, const char* longName, const char* shortName, const MDistance& value, int32_t flags)
{
	MFnUnitAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createTimeAttribute(MObject& outAttr, const char* longName, const char* shortName, const MTime& value, int32_t flags)
{
	MFnUnitAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, value);
	setAttributeFlags(fnAttr, flags);
}

// ------ MFnMatrixAttribute ------
void NodeHelper::createFloatMatrixAttribute(MObject& outAttr, const char* longName, const char* shortName, const MFloatMatrix& value, int32_t flags)
{
	MFnMatrixAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnMatrixAttribute::kFloat);
	fnAttr.setDefault(value);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createMatrixAttribute(MObject& outAttr, const char* longName, const char* shortName, const MMatrix& value, int32_t flags)
{
	MFnMatrixAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnMatrixAttribute::kDouble);
	fnAttr.setDefault(value);
	setAttributeFlags(fnAttr, flags);
}
	
// ------ MFnMessageAttribute ------
void NodeHelper::createMessageAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	MFnMessageAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName);
	setAttributeFlags(fnAttr, flags);
}

// ------ MFnEnumAttribute ------

/*	This is a pre-defined enum attribute, providing the user with a range of easing options
	The selected option can be used to retrieve an interpolation function from the MRS::easingFuncMap

	linear			(0)		Linear interpolation, constant velocity
	inSine			(1)		Sinusoidal easing in, accelerating from zero velocity
	inQuad			(2)		Quadratic easing in, accelerating from zero velocity
	inCubic			(3)		Cubic easing in, accelerating from zero velocity
	inQuart			(4)		Quartic easing in, accelerating from zero velocity
	inQuint			(5)		Quintic easing in, accelerating from zero velocity
	inExpo			(6)		Exponential easing in, accelerating from zero velocity
	inCirc			(7)		Circular easing in, accelerating from zero velocity
	outSine			(8)		Sinusoidal easing out, decelerating to zero velocity
	outQuad			(9)		Quadratic easing out, decelerating to zero velocity
	outCubic		(10)	Cubic easing out, decelerating to zero velocity
	outQuart		(11)	Quartic easing out, decelerating to zero velocity
	outQuint		(12)	Quintic easing out, decelerating to zero velocity
	outExpo			(13)	Exponential easing out, decelerating to zero velocity
	outCirc			(14)	Circular easing out, decelerating to zero velocity
	inOutSine		(15)	Sinusoidal easing in and out, accelerating until halfway, then decelerating
	inOutQuad		(16)	Quadratic easing in and out, accelerating until halfway, then decelerating
	inOutCubic		(17)	Cubic easing in and out, accelerating until halfway, then decelerating
	inOutQuart		(18)	Quartic easing in and out, accelerating until halfway, then decelerating
	inOutQuint		(19)	Quintic easing in and out, accelerating until halfway, then decelerating
	inOutExpo		(20)	Exponential easing in and out, accelerating until halfway, then decelerating
	inOutCirc		(21)	Circular easing in and out, accelerating until halfway, then decelerating	*/
void NodeHelper::createEasingAttribute(MObject& outAttr, const char* longName, const char* shortName, MRS::Easing defaultValue, int32_t flags)
{
	MFnEnumAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, defaultValue);
	static const std::unordered_map<const char*, MRS::Easing> fields = {
		{"linear", MRS::Easing::kLinear}, {"inSine", MRS::Easing::kInSine}, {"inQuad", MRS::Easing::kInQuad}, {"inCubic", MRS::Easing::kInCubic},
		{"inQuart", MRS::Easing::kInQuart}, {"inQuint", MRS::Easing::kInQuint}, {"inExpo", MRS::Easing::kInExpo}, {"inCirc", MRS::Easing::kInCirc},
		{"outSine", MRS::Easing::kOutSine}, {"outQuad", MRS::Easing::kOutQuad}, {"outCubic", MRS::Easing::kOutCubic}, {"outQuart", MRS::Easing::kOutQuart},
		{"outQuint", MRS::Easing::kOutQuint}, {"outExpo", MRS::Easing::kOutExpo}, {"outCirc", MRS::Easing::kOutCirc}, {"inOutSine", MRS::Easing::kInOutSine},
		{"inOutQuad", MRS::Easing::kInOutQuad}, {"inOutCubic", MRS::Easing::kInOutCubic}, {"inOutQuart", MRS::Easing::kInOutQuart},
		{"inOutQuint", MRS::Easing::kInOutQuint}, {"inOutExpo", MRS::Easing::kInOutExpo}, {"inOutCirc", MRS::Easing::kInOutCirc},
	};

	for (std::unordered_map<const char*, MRS::Easing>::const_iterator it = fields.begin(); it != fields.end(); ++it)
		fnAttr.addField(it->first, it->second);

	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createEnumAttribute(MObject& outAttr, const char* longName, const char* shortName, std::unordered_map<const char*, short>& fields, short defaultValue, int32_t flags)
{
	MFnEnumAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, defaultValue);

	for (std::unordered_map<const char*, short>::iterator it = fields.begin(); it != fields.end(); ++it)
		fnAttr.addField(it->first, it->second);

	setAttributeFlags(fnAttr, flags);
}

// ------ MFnCompoundAttribute ------
void NodeHelper::createQuaternionAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr, MObject& outChildWAttr,
	const char* longName, const char* shortName, const MQuaternion& value, int32_t flags)
{
	MFnCompoundAttribute fnCAttr;
	MFnNumericAttribute fnNAttr;

	const std::string attrXShortName = (std::string(shortName) + "X");
	const std::string attrXLongName = (std::string(longName) + "X");
	outChildXAttr = fnNAttr.create(attrXShortName.c_str(), attrXLongName.c_str(), MFnNumericData::kDouble, value.x);
	setChildAttributeFlags(fnNAttr, flags);

	const std::string attrYShortName = (std::string(shortName) + "Y");
	const std::string attrYLongName = (std::string(longName) + "Y");
	outChildYAttr = fnNAttr.create(attrYShortName.c_str(), attrYLongName.c_str(), MFnNumericData::kDouble, value.y);
	setChildAttributeFlags(fnNAttr, flags);

	const std::string attrZShortName = (std::string(shortName) + "Z");
	const std::string attrZLongName = (std::string(longName) + "Z");
	outChildZAttr = fnNAttr.create(attrZShortName.c_str(), attrZLongName.c_str(), MFnNumericData::kDouble, value.z);
	setChildAttributeFlags(fnNAttr, flags);

	const std::string attrWShortName = (std::string(shortName) + "W");
	const std::string attrWLongName = (std::string(longName) + "W");
	outChildWAttr = fnNAttr.create(attrWShortName.c_str(), attrWLongName.c_str(), MFnNumericData::kDouble, value.w);
	setChildAttributeFlags(fnNAttr, flags);

	outParentAttr = fnCAttr.create(longName, shortName);
	fnCAttr.addChild(outChildXAttr);
	fnCAttr.addChild(outChildYAttr);
	fnCAttr.addChild(outChildZAttr);
	fnCAttr.addChild(outChildWAttr);
	setAttributeFlags(fnCAttr, flags);
}

void NodeHelper::createEulerAttribute(MObject& outParentAttr, MObject& outChildXAttr, MObject& outChildYAttr, MObject& outChildZAttr,
	const char* longName, const char* shortName, const MEulerRotation& value, int32_t flags)
{
	MFnNumericAttribute fnNAttr;
	MFnUnitAttribute fnUAttr;

	const std::string attrXShortName = (std::string(shortName) + "X");
	const std::string attrXLongName = (std::string(longName) + "X");
	outChildXAttr = fnUAttr.create(attrXShortName.c_str(), attrXLongName.c_str(), MAngle(value.x));
	setChildAttributeFlags(fnUAttr, flags);

	const std::string attrYShortName = (std::string(shortName) + "Y");
	const std::string attrYLongName = (std::string(longName) + "Y");
	outChildYAttr = fnUAttr.create(attrYShortName.c_str(), attrYLongName.c_str(), MAngle(value.y));
	setChildAttributeFlags(fnUAttr, flags);

	const std::string attrZShortName = (std::string(shortName) + "Z");
	const std::string attrZLongName = (std::string(longName) + "Z");
	outChildZAttr = fnUAttr.create(attrZShortName.c_str(), attrZLongName.c_str(), MAngle(value.z));
	setChildAttributeFlags(fnUAttr, flags);

	outParentAttr = fnNAttr.create(longName, shortName, outChildXAttr, outChildYAttr, outChildZAttr);
	setAttributeFlags(fnNAttr, flags);
}

void NodeHelper::createCompoundAttribute(MObject& outParentAttr, const std::vector<MObject>& children, const char* longName, const char* shortName, int32_t flags)
{
	MFnCompoundAttribute fnAttr;
	outParentAttr = fnAttr.create(longName, shortName);

	for (const MObject& childAttr : children)
		fnAttr.addChild(childAttr);

	// Child attributes are expected to have their own flags set
	setAttributeFlags(fnAttr, flags);
}

// ------ MFnTypedAttribute  ------
void NodeHelper::createIntDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<int>& value, int32_t flags)
{
	MIntArray dataArray;
	MFnIntArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kIntArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createFloatDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<float>& value, int32_t flags)
{
	MFloatArray dataArray;
	MFnFloatArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kFloatArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createDoubleDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<double>& value, int32_t flags)
{
	MDoubleArray dataArray;
	MFnDoubleArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kDoubleArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}
	

void NodeHelper::createVectorDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MVector>& value, int32_t flags)
{
	MVectorArray dataArray;
	MFnVectorArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kVectorArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}


void NodeHelper::createPointDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MPoint>& value, int32_t flags)
{
	MPointArray dataArray;
	MFnPointArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kPointArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createMatrixDataArrayAttribute(MObject& outAttr, const char* longName, const char* shortName, std::vector<MMatrix>& value, int32_t flags)
{
	MMatrixArray dataArray;
	MFnMatrixArrayData fnData;
	MFnTypedAttribute fnAttr;

	dataArray.setLength((unsigned)value.size());
	for (unsigned int i = 0; i < value.size(); i++) {
		dataArray[i] = value[i];
	}

	MObject dataObj = fnData.create(dataArray);
	outAttr = fnAttr.create(longName, shortName, MFnData::kMatrixArray, dataObj);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createNurbsCurveAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	MFnTypedAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnData::kNurbsCurve);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createNurbsSurfaceAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	MFnTypedAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnData::kNurbsSurface);
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createMeshAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	MFnTypedAttribute fnAttr;
	outAttr = fnAttr.create(longName, shortName, MFnData::kMesh);
	setAttributeFlags(fnAttr, flags);
}

// ------ MRampAttribute ------

// Note, an intial value should be added in MPxNode::postConstructor() to avoid errors in the Attribute Editor
void NodeHelper::createCurveRampAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	outAttr = MRampAttribute::createCurveRamp(longName, shortName);
	MFnAttribute fnAttr{ outAttr };
	setAttributeFlags(fnAttr, flags);
}

void NodeHelper::createColorRampAttribute(MObject& outAttr, const char* longName, const char* shortName, int32_t flags)
{
	outAttr = MRampAttribute::createColorRamp(longName, shortName);
	MFnAttribute fnAttr{ outAttr };
	setAttributeFlags(fnAttr, flags);
}

// ------ Get ----------------------------------------------------------------------------------------------------------------------------------------------------

// ------ MFnNumericAttribute ------
bool NodeHelper::inputBoolValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asBool();
}

int NodeHelper::inputIntValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asInt();
}

float NodeHelper::inputFloatValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asFloat();
}

double NodeHelper::inputDoubleValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asDouble();
}

MFloatVector NodeHelper::inputFloatVectorValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asFloatVector();
}

MVector NodeHelper::inputVectorValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asVector();
}

std::vector<bool> NodeHelper::inputBoolArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<bool> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	// Uses logical indices (may be sparse)
	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asBool());
		inArrayHandle.next();
	}

	return values;
}

std::vector<int> NodeHelper::inputIntArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<int> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asInt());
		inArrayHandle.next();
	}

	return values;
}

std::vector<float> NodeHelper::inputFloatArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<float> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asFloat());
		inArrayHandle.next();
	}

	return values;
}

std::vector<double> NodeHelper::inputDoubleArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<double> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asDouble());
		inArrayHandle.next();
	}

	return values;
}

std::vector<MFloatVector> NodeHelper::inputFloatVectorArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MFloatVector> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asFloatVector());
		inArrayHandle.next();
	}

	return values;
}

std::vector<MVector> NodeHelper::inputVectorArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MVector> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asVector());
		inArrayHandle.next();
	}

	return values;
}

// ------ MFnUnitAttribute ------
MAngle NodeHelper::inputAngleValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asAngle();
}

MDistance NodeHelper::inputDistanceValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asDistance();
}

MTime NodeHelper::inputTimeValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asTime();
}

std::vector<MAngle> NodeHelper::inputAngleArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MAngle> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asAngle());
		inArrayHandle.next();
	}

	return values;
}

std::vector<MDistance> NodeHelper::inputDistanceArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MDistance> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asDistance());
		inArrayHandle.next();
	}

	return values;
}

std::vector<MTime> NodeHelper::inputTimeArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MTime> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asTime());
		inArrayHandle.next();
	}

	return values;
}

// ------ MFnMatrixAttribute ------
MFloatMatrix NodeHelper::inputFloatMatrixValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asFloatMatrix();
}

MMatrix NodeHelper::inputMatrixValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asMatrix();
}

std::vector<MFloatMatrix> NodeHelper::inputFloatMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MFloatMatrix> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asFloatMatrix());
		inArrayHandle.next();
	}

	return values;
}

std::vector<MMatrix> NodeHelper::inputMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MMatrix> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asMatrix());
		inArrayHandle.next();
	}

	return values;
}

// ------ MFnEnumAttribute ------
MRS::Easing NodeHelper::inputEasingValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return (MRS::Easing)inHandle.asShort();
}

short NodeHelper::inputEnumValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asShort();
}

std::vector<MRS::Easing> NodeHelper::inputEasingArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MRS::Easing> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back((MRS::Easing)inArrayHandle.inputValue().asShort());
		inArrayHandle.next();
	}

	return values;
}

std::vector<short> NodeHelper::inputEnumArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<short> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(attr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		values.push_back(inArrayHandle.inputValue().asShort());
		inArrayHandle.next();
	}

	return values;
}

// ------ MFnCompoundAttribute ------
std::vector<bool> NodeHelper::inputBoolCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children)
{
	std::vector<bool> values;
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);
		
	values.reserve(children.size());
	for (unsigned int i = 0; i < children.size(); i++)
	{
		values.push_back(inHandle.child(children[i]).asBool());
	}

	return values;
}

std::vector<int> NodeHelper::inputIntCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children)
{
	std::vector<int> values;
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);

	values.reserve(children.size());
	for (unsigned int i = 0; i < children.size(); i++)
	{
		values.push_back(inHandle.child(children[i]).asInt());
	}

	return values;
}

std::vector<float> NodeHelper::inputFloatCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children)
{
	std::vector<float> values;
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);

	values.reserve(children.size());
	for (unsigned int i = 0; i < children.size(); i++)
	{
		values.push_back(inHandle.child(children[i]).asFloat());
	}

	return values;
}

std::vector<double> NodeHelper::inputDoubleCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children)
{
	std::vector<double> values;
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);

	values.reserve(children.size());
	for (unsigned int i = 0; i < children.size(); i++)
	{
		values.push_back(inHandle.child(children[i]).asDouble());
	}

	return values;
}

MQuaternion NodeHelper::inputQuaternionValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr, const MObject childWAttr)
{
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);

	const double x = inHandle.child(childXAttr).asDouble();
	const double y = inHandle.child(childYAttr).asDouble();
	const double z = inHandle.child(childZAttr).asDouble();
	const double w = inHandle.child(childWAttr).asDouble();

	return MQuaternion(x, y, z, w);
}

MEulerRotation NodeHelper::inputEulerValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr)
{
	MDataHandle inHandle = dataBlock.inputValue(parentAttr);

	const double x = inHandle.child(childXAttr).asAngle().asRadians();
	const double y = inHandle.child(childYAttr).asAngle().asRadians();
	const double z = inHandle.child(childZAttr).asAngle().asRadians();

	return MEulerRotation(x, y, z);
}

std::vector<MQuaternion> NodeHelper::inputQuaternionArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr, const MObject childWAttr)
{
	std::vector<MQuaternion> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(parentAttr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		MDataHandle inElementHandle = inArrayHandle.inputValue();
		const double x = inElementHandle.child(childXAttr).asDouble();
		const double y = inElementHandle.child(childYAttr).asDouble();
		const double z = inElementHandle.child(childZAttr).asDouble();
		const double w = inElementHandle.child(childWAttr).asDouble();
		values.emplace_back(x, y, z, w);
		inArrayHandle.next();
	}

	return values;
}

std::vector<MEulerRotation> NodeHelper::inputEulerArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr)
{
	std::vector<MEulerRotation> values;
	MArrayDataHandle inArrayHandle = dataBlock.inputArrayValue(parentAttr);

	values.reserve(inArrayHandle.elementCount());
	for (unsigned int i = 0; i < inArrayHandle.elementCount(); i++)
	{
		MDataHandle inElementHandle = inArrayHandle.inputValue();
		const double x = inElementHandle.child(childXAttr).asAngle().asRadians();
		const double y = inElementHandle.child(childYAttr).asAngle().asRadians();
		const double z = inElementHandle.child(childZAttr).asAngle().asRadians();
		values.emplace_back(x, y, z);
		inArrayHandle.next();
	}

	return values;
}

// ------ MFnTypedAttribute ------
std::vector<int> NodeHelper::inputIntDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<int> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnIntArrayData fnIntArrayData(dataObj);
	values.reserve(fnIntArrayData.length());

	for (unsigned int i = 0; i < fnIntArrayData.length(); i++) {
		values.push_back(fnIntArrayData[i]);
	}

	return values;
}

std::vector<float> NodeHelper::inputFloatDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<float> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnFloatArrayData fnData(dataObj);
	values.reserve(fnData.length());

	for (unsigned int i = 0; i < fnData.length(); i++) {
		values.push_back(fnData[i]);
	}

	return values;
}

std::vector<double> NodeHelper::inputDoubleDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<double> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnDoubleArrayData fnData(dataObj);
	values.reserve(fnData.length());

	for (unsigned int i = 0; i < fnData.length(); i++) {
		values.push_back(fnData[i]);
	}

	return values;
}

std::vector<MVector> NodeHelper::inputVectorDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MVector> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnVectorArrayData fnData(dataObj);
	values.reserve(fnData.length());

	for (unsigned int i = 0; i < fnData.length(); i++) {
		values.push_back(fnData[i]);
	}

	return values;
}

std::vector<MPoint> NodeHelper::inputPointDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MPoint> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnPointArrayData fnData(dataObj);
	values.reserve(fnData.length());

	for (unsigned int i = 0; i < fnData.length(); i++) {
		values.push_back(fnData[i]);
	}

	return values;
}

std::vector<MMatrix> NodeHelper::inputMatrixDataArrayValue(MDataBlock& dataBlock, const MObject& attr)
{
	std::vector<MMatrix> values;
	MDataHandle inHandle = dataBlock.inputValue(attr);
	MObject dataObj = inHandle.data();
	MFnMatrixArrayData fnData(dataObj);
	values.reserve(fnData.length());

	for (unsigned int i = 0; i < fnData.length(); i++) {
		values.push_back(fnData[i]);
	}

	return values;
}

MObject NodeHelper::inputNurbsCurveValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asNurbsCurve();
}

MObject NodeHelper::inputNurbsSurfaceValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asNurbsSurface();
}

MObject NodeHelper::inputMeshValue(MDataBlock& dataBlock, const MObject& attr)
{
	MDataHandle inHandle = dataBlock.inputValue(attr);
	return inHandle.asMesh();
}


// ------ MRampAttribute ------

/*	Notes
	-----
	This attribute type uses a custom class to store retrieved data
	The standard way of retrieving data from a ramp attribute is to retrieve the plug and instantiate a MRampAttribute object which can then be queried
	However during computation of a node/deformer, this is not considered safe
	Fortunately Walt Disney Animation Studios provides an open source expression language which has equivalent functionality and can be used safely during computation
*/

SeExpr2::Curve<double> NodeHelper::inputCurveRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, 
	MObject& valueAttr, MObject& interpAttr)
{
	SeExpr2::Curve<double> curve;
	MArrayDataHandle inCompoundArrayHandle = dataBlock.inputArrayValue(parentAttr);
	
	for (unsigned int i = 0; i < inCompoundArrayHandle.elementCount(); ++i)
	{
		MDataHandle inCompoundElementHandle = inCompoundArrayHandle.inputValue();

		float position = inCompoundElementHandle.child(positionAttr).asFloat();
		float value = inCompoundElementHandle.child(valueAttr).asFloat();
		short interp = inCompoundElementHandle.child(interpAttr).asShort();

		curve.addPoint((double)position, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp));
		inCompoundArrayHandle.next();
	}

	// Ensures data is sorted and boundaries are clamped
	curve.preparePoints();
	return curve;
}

SeExpr2::Curve<SeExpr2::Vec3d> NodeHelper::inputColorRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, 
	MObject& colorAttr, MObject& interpAttr)
{
	SeExpr2::Curve<SeExpr2::Vec3d> curve;
	MArrayDataHandle inCompoundArrayHandle = dataBlock.inputArrayValue(parentAttr);

	for (unsigned int i = 0; i < inCompoundArrayHandle.elementCount(); ++i)
	{
		MDataHandle inCompoundElementHandle = inCompoundArrayHandle.inputValue();

		float position = inCompoundElementHandle.child(positionAttr).asFloat();
		MFloatVector color = inCompoundElementHandle.child(colorAttr).asFloatVector();
		SeExpr2::Vec3d colorVec{ color.x, color.y, color.z };
		short interp = inCompoundElementHandle.child(interpAttr).asShort();

		curve.addPoint((double)position, colorVec, static_cast<SeExpr2::Curve<SeExpr2::Vec3d>::InterpType>(interp));
		inCompoundArrayHandle.next();
	}

	// Ensures data is sorted and boundaries are clamped
	curve.preparePoints();
	return curve;
}

// ------ Set ------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ MFnNumericAttribute ------
void NodeHelper::outputBoolValue(MDataBlock& dataBlock, const MObject& attr, const bool value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setBool(value);
	outHandle.setClean();
}

void NodeHelper::outputIntValue(MDataBlock& dataBlock, const MObject& attr, const int value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setInt(value);
	outHandle.setClean();
}

void NodeHelper::outputFloatValue(MDataBlock& dataBlock, const MObject& attr, const float value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setFloat(value);
	outHandle.setClean();
}

void NodeHelper::outputDoubleValue(MDataBlock& dataBlock, const MObject& attr, const double value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setDouble(value);
	outHandle.setClean();
}

void NodeHelper::outputFloatVectorValue(MDataBlock& dataBlock, const MObject& attr, const MFloatVector& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMFloatVector(value);
	outHandle.setClean();
}

void NodeHelper::outputVectorValue(MDataBlock& dataBlock, const MObject& attr, const MVector& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMVector(value);
	outHandle.setClean();
}

void NodeHelper::outputBoolArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<bool>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());
	
	for (const bool& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setBool(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputIntArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<int>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const int& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setInt(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputFloatArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<float>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const float& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setFloat(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputDoubleArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<double>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const double& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setDouble(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputFloatVectorArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MFloatVector>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MFloatVector& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMFloatVector(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputVectorArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MVector>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MVector& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMVector(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ MFnUnitAttribute ------
void NodeHelper::outputAngleValue(MDataBlock& dataBlock, const MObject& attr, const MAngle& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMAngle(value);
	outHandle.setClean();
}

void NodeHelper::outputDistanceValue(MDataBlock& dataBlock, const MObject& attr, const MDistance& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMDistance(value);
	outHandle.setClean();
}

void NodeHelper::outputTimeValue(MDataBlock& dataBlock, const MObject& attr, const MTime& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMTime(value);
	outHandle.setClean();
}

void NodeHelper::outputAngleArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MAngle>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MAngle& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMAngle(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputDistanceArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MDistance>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MDistance& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMDistance(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputTimeArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MTime>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MTime& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMTime(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ MFnMatrixAttribute ------
void NodeHelper::outputFloatMatrixValue(MDataBlock& dataBlock, const MObject& attr, const MFloatMatrix& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMFloatMatrix(value);
	outHandle.setClean();
}

void NodeHelper::outputMatrixValue(MDataBlock& dataBlock, const MObject& attr, const MMatrix& value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setMMatrix(value);
	outHandle.setClean();
}

void NodeHelper::outputFloatMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MFloatMatrix>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MFloatMatrix& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMFloatMatrix(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputMatrixArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MMatrix>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MMatrix& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setMMatrix(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ MFnEnumAttribute ------
void NodeHelper::outputEasingValue(MDataBlock& dataBlock, const MObject& attr, const MRS::Easing value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setShort((short)value);
	outHandle.setClean();
}

void NodeHelper::outputEnumValue(MDataBlock& dataBlock, const MObject& attr, const short value)
{
	MDataHandle outHandle = dataBlock.outputValue(attr);
	outHandle.setShort(value);
	outHandle.setClean();
}

void NodeHelper::outputEasingArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MRS::Easing>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const MRS::Easing& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setShort((short)value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputEnumArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<short>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(attr);
	MArrayDataBuilder builder(&dataBlock, attr, (unsigned)values.size());

	for (const short& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();
		outElementHandle.setShort(value);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ MFnCompoundAttribute ------
void NodeHelper::outputBoolCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<bool>& values)
{
	unsigned int childCount = unsigned(children.size());
	assert(childCount == values.size());

	MDataHandle outParentHandle = dataBlock.outputValue(parentAttr);

	for (unsigned int i = 0; i < childCount; i++)
	{
		MDataHandle outChildHandle = outParentHandle.child(children[i]);
		outChildHandle.setBool(values[i]);
		outChildHandle.setClean();
	}
}

void NodeHelper::outputIntCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<int>& values)
{
	unsigned int childCount = unsigned(children.size());
	assert(childCount == values.size());

	MDataHandle outParentHandle = dataBlock.outputValue(parentAttr);

	for (unsigned int i = 0; i < childCount; i++)
	{
		MDataHandle outChildHandle = outParentHandle.child(children[i]);
		outChildHandle.setInt(values[i]);
		outChildHandle.setClean();
	}
}

void NodeHelper::outputFloatCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<float>& values)
{
	unsigned int childCount = unsigned(children.size());
	assert(childCount == values.size());

	MDataHandle outParentHandle = dataBlock.outputValue(parentAttr);

	for (unsigned int i = 0; i < childCount; i++)
	{
		MDataHandle outChildHandle = outParentHandle.child(children[i]);
		outChildHandle.setFloat(values[i]);
		outChildHandle.setClean();
	}
}

void NodeHelper::outputDoubleCompoundValue(MDataBlock& dataBlock, const MObject& parentAttr, const std::vector<MObject>& children, const std::vector<double>& values)
{
	unsigned int childCount = unsigned(children.size());
	assert(childCount == values.size());

	MDataHandle outParentHandle = dataBlock.outputValue(parentAttr);

	for (unsigned int i = 0; i < childCount; i++)
	{
		MDataHandle outChildHandle = outParentHandle.child(children[i]);
		outChildHandle.setDouble(values[i]);
		outChildHandle.setClean();
	}
}

void NodeHelper::outputQuaternionValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr, const MObject childWAttr, const MQuaternion& value)
{
	MDataHandle outHandle = dataBlock.outputValue(parentAttr);

	MDataHandle outChildXHandle = outHandle.child(childXAttr);
	MDataHandle outChildYHandle = outHandle.child(childYAttr);
	MDataHandle outChildZHandle = outHandle.child(childZAttr);
	MDataHandle outChildWHandle = outHandle.child(childWAttr);

	outChildXHandle.set(value.x);
	outChildYHandle.set(value.y);
	outChildZHandle.set(value.z);
	outChildWHandle.set(value.w);

	outChildXHandle.setClean();
	outChildYHandle.setClean();
	outChildZHandle.setClean();
	outChildWHandle.setClean();
}

void NodeHelper::outputEulerValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject childXAttr, const MObject childYAttr,
	const MObject childZAttr, const MEulerRotation& value)
{
	MDataHandle outParentHandle = dataBlock.outputValue(parentAttr);

	MDataHandle outChildXHandle = outParentHandle.child(childXAttr);
	MDataHandle outChildYHandle = outParentHandle.child(childYAttr);
	MDataHandle outChildZHandle = outParentHandle.child(childZAttr);

	outChildXHandle.set(MAngle(value.x));
	outChildYHandle.set(MAngle(value.y));
	outChildZHandle.set(MAngle(value.z));

	outChildXHandle.setClean();
	outChildYHandle.setClean();
	outChildZHandle.setClean();
}

void NodeHelper::outputQuaternionArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject& childXAttr, const MObject& childYAttr,
	const MObject& childZAttr, const MObject& childWAttr, const std::vector<MQuaternion>& values)
{
	unsigned int elementCount = unsigned(values.size());
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(parentAttr);
	MArrayDataBuilder builder(&dataBlock, parentAttr, unsigned(elementCount));

	for (unsigned int i = 0; i < elementCount; i++)
	{
		MDataHandle outElementHandle = builder.addLast();

		MDataHandle outChildXHandle = outElementHandle.child(childXAttr);
		MDataHandle outChildYHandle = outElementHandle.child(childYAttr);
		MDataHandle outChildZHandle = outElementHandle.child(childZAttr);
		MDataHandle outChildWHandle = outElementHandle.child(childWAttr);

		outChildXHandle.set(values[i].x);
		outChildYHandle.set(values[i].y);
		outChildZHandle.set(values[i].z);
		outChildWHandle.set(values[i].w);
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputEulerArrayValue(MDataBlock& dataBlock, const MObject& parentAttr, const MObject& childXAttr, const MObject& childYAttr,
	const MObject& childZAttr, const std::vector<MEulerRotation>& values)
{
	unsigned int elementCount = unsigned(values.size());
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(parentAttr);
	MArrayDataBuilder builder(&dataBlock, parentAttr, unsigned(elementCount));

	for (unsigned int i = 0; i < elementCount; i++)
	{
		MDataHandle outElementHandle = builder.addLast();

		MDataHandle outChildXHandle = outElementHandle.child(childXAttr);
		MDataHandle outChildYHandle = outElementHandle.child(childYAttr);
		MDataHandle outChildZHandle = outElementHandle.child(childZAttr);

		outChildXHandle.set(MAngle(values[i].x));
		outChildYHandle.set(MAngle(values[i].y));
		outChildZHandle.set(MAngle(values[i].z));
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ MFnTypedAttribute ------
void NodeHelper::outputIntDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<int>& values)
{
	unsigned int size = (int)values.size();
	MDataHandle outHandle = dataBlock.outputValue(attr);
	MFnIntArrayData fnData;
	MIntArray outDataArray;
	outDataArray.setLength(size);

	for (unsigned int i = 0; i < size; i++)
		outDataArray[i] = values[i];

	MObject outObj = fnData.create(outDataArray);
	outHandle.setMObject(outObj);
	outHandle.setClean();
}

void NodeHelper::outputFloatDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<float>& values)
{
	{
		unsigned int size = (int)values.size();
		MDataHandle outHandle = dataBlock.outputValue(attr);
		MFnFloatArrayData fnData;
		MFloatArray outDataArray;
		outDataArray.setLength(size);

		for (unsigned int i = 0; i < size; i++)
			outDataArray[i] = values[i];

		MObject outObj = fnData.create(outDataArray);
		outHandle.setMObject(outObj);
		outHandle.setClean();
	}
}

void NodeHelper::outputDoubleDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<double>& values)
{
	unsigned int size = (int)values.size();
	MDataHandle outHandle = dataBlock.outputValue(attr);
	MFnDoubleArrayData fnData;
	MDoubleArray outDataArray;
	outDataArray.setLength(size);

	for (unsigned int i = 0; i < size; i++)
		outDataArray[i] = values[i];

	MObject outObj = fnData.create(outDataArray);
	outHandle.setMObject(outObj);
	outHandle.setClean();
}

void NodeHelper::outputVectorDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MVector>& values)
{
	unsigned int size = (int)values.size();
	MDataHandle outHandle = dataBlock.outputValue(attr);
	MFnVectorArrayData fnData;
	MVectorArray outDataArray;
	outDataArray.setLength(size);

	for (unsigned int i = 0; i < size; i++)
		outDataArray[i] = values[i];

	MObject outObj = fnData.create(outDataArray);
	outHandle.setMObject(outObj);
	outHandle.setClean();
}

void NodeHelper::outputPointDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MPoint>& values)
{
	unsigned int size = (int)values.size();
	MDataHandle outHandle = dataBlock.outputValue(attr);
	MFnPointArrayData fnData;
	MPointArray outDataArray;
	outDataArray.setLength(size);

	for (unsigned int i = 0; i < size; i++)
		outDataArray[i] = values[i];

	MObject outObj = fnData.create(outDataArray);
	outHandle.setMObject(outObj);
	outHandle.setClean();
}

void NodeHelper::outputMatrixDataArrayValue(MDataBlock& dataBlock, const MObject& attr, const std::vector<MMatrix>& values)
{
	unsigned int size = (int)values.size();
	MDataHandle outHandle = dataBlock.outputValue(attr);
	MFnMatrixArrayData fnData;
	MMatrixArray outDataArray;
	outDataArray.setLength(size);

	for (unsigned int i = 0; i < size; i++)
		outDataArray[i] = values[i];

	MObject outObj = fnData.create(outDataArray);
	outHandle.setMObject(outObj);
	outHandle.setClean();
}

// ------ MRampAttribute ------

void NodeHelper::outputCurveRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, MObject& valueAttr, MObject& interpAttr, 
	std::vector<SeExpr2::Curve<double>::CV>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(parentAttr);
	MArrayDataBuilder builder(&dataBlock, parentAttr, (unsigned)values.size());

	for (SeExpr2::Curve<double>::CV& value: values)
	{
		MDataHandle outElementHandle = builder.addLast();

		MDataHandle outChildPositionHandle = outElementHandle.child(positionAttr);
		MDataHandle outChildValueHandle = outElementHandle.child(valueAttr);
		MDataHandle outChildInterpHandle = outElementHandle.child(interpAttr);

		outChildPositionHandle.setFloat((float)value._pos);
		outChildValueHandle.setFloat((float)value._val);
		outChildInterpHandle.setShort(static_cast<short>(value._interp));
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

void NodeHelper::outputColorRampAttribute(MDataBlock& dataBlock, const MObject& parentAttr, MObject& positionAttr, MObject& colorAttr, MObject& interpAttr,
	std::vector<SeExpr2::Curve<SeExpr2::Vec3d>::CV>& values)
{
	MArrayDataHandle outArrayHandle = dataBlock.outputArrayValue(parentAttr);
	MArrayDataBuilder builder(&dataBlock, parentAttr, (unsigned)values.size());

	for (SeExpr2::Curve<SeExpr2::Vec3d>::CV& value : values)
	{
		MDataHandle outElementHandle = builder.addLast();

		MDataHandle outChildPositionHandle = outElementHandle.child(positionAttr);
		MDataHandle outChildColorHandle = outElementHandle.child(colorAttr);
		MDataHandle outChildInterpHandle = outElementHandle.child(interpAttr);

		outChildPositionHandle.setFloat((float)value._pos);
		outChildColorHandle.set3Float((float)value._val[0], (float)value._val[1], (float)value._val[2]);
		outChildInterpHandle.setShort(static_cast<short>(value._interp));
	}

	outArrayHandle.set(builder);
	outArrayHandle.setAllClean();
}

// ------ Utilities ------
void NodeHelper::setNiceName(MObject& attr, const char* niceName)
{
	MFnAttribute(attr).setNiceNameOverride(niceName);
}

// ------ Helpers ------
void NodeHelper::setAttributeFlags(MFnAttribute& fnAttr, int32_t flags)
{
	fnAttr.setReadable(flags & kReadable);
	fnAttr.setWritable(flags & kWritable);
	fnAttr.setConnectable(flags & kConnectable);
	fnAttr.setStorable(flags & kStorable);
	fnAttr.setCached(flags & kCached);
	fnAttr.setArray(flags & kArray);
	fnAttr.setIndexMatters(flags & kIndexMatters);
	fnAttr.setKeyable(flags & kKeyable);
	fnAttr.setChannelBox(flags & kChannelBox);
	fnAttr.setHidden(flags & kHidden);
	fnAttr.setUsedAsColor(flags & kUsedAsColor);
	fnAttr.setIndeterminant(flags & kIndeterminant);
	fnAttr.setRenderSource(flags & kRenderSource);
	fnAttr.setWorldSpace(flags & kWorldSpace);
	fnAttr.setAffectsWorldSpace(flags & kAffectsWorldSpace);
	fnAttr.setUsedAsFilename(flags & kUsedAsFileName);
	fnAttr.setAffectsAppearance(flags & kAffectsAppearance);
	fnAttr.setProxyAttribute(flags & kProxyAttribute);
	fnAttr.setUsesArrayDataBuilder(flags & kUsesArrayDataBuilder);
	fnAttr.setInternal(flags & kInternal);

	if (flags & kDisconnectBehaviorDelete)
		fnAttr.setDisconnectBehavior(MFnAttribute::DisconnectBehavior::kDelete);
	else if (flags & kDisconnectBehaviorReset)
		fnAttr.setDisconnectBehavior(MFnAttribute::DisconnectBehavior::kReset);
	else if (flags & kDisconnectBehaviorNothing)
		fnAttr.setDisconnectBehavior(MFnAttribute::DisconnectBehavior::kNothing);
}

void NodeHelper::setChildAttributeFlags(MFnAttribute& fnAttr, int32_t flags)
{
	fnAttr.setReadable(flags & kReadable);
	fnAttr.setWritable(flags & kWritable);
	fnAttr.setConnectable(flags & kConnectable);
	fnAttr.setStorable(flags & kStorable);
	fnAttr.setCached(flags & kCached);
	fnAttr.setKeyable(flags & kKeyable);
	fnAttr.setChannelBox(flags & kChannelBox);
	fnAttr.setHidden(flags & kHidden);
	fnAttr.setUsedAsColor(flags & kUsedAsColor);
	fnAttr.setIndeterminant(flags & kIndeterminant);
	fnAttr.setRenderSource(flags & kRenderSource);
	fnAttr.setAffectsWorldSpace(flags & kAffectsWorldSpace);
	fnAttr.setUsedAsFilename(flags & kUsedAsFileName);
	fnAttr.setAffectsAppearance(flags & kAffectsAppearance);
	fnAttr.setProxyAttribute(flags & kProxyAttribute);
	fnAttr.setInternal(flags & kInternal);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------