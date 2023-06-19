#include "angleArray_data.h"
#include "utils/macros.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AngleArrayData::AngleArrayData() : MRS::DataArrayHelper<MAngle, double, 1>() {};

AngleArrayData::~AngleArrayData() {};

// ------ Const ------
// These must be defined outside of plugin.cpp, as the MTypeId is required by targets which link to the static data library
// Project Block0 IDs : [0x00130fc0 - 0x00130fcf]
const MTypeId AngleArrayData::kTypeId = 0x00130fc2;

const MString AngleArrayData::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AngleArray";

// ------ MPxData ------
MTypeId AngleArrayData::typeId() const
{
	return AngleArrayData::kTypeId;
}

MString AngleArrayData::name() const
{
	return AngleArrayData::kTypeName;
}

// ------ MRS::DataArrayHelper ------
unsigned int AngleArrayData::length() const
{
	return MRS::DataArrayHelper<MAngle, double, 1>::length();
}

// Used by get attribute methods
void AngleArrayData::getArray(std::vector<MAngle>& outArray) const
{
	unsigned int arrayLength = length();
	outArray.resize(arrayLength);

	for (unsigned int i = 0; i < arrayLength; i++) {
		outArray[i] = MAngle(m_data[i]);
	}
}

// Used by set attribute methods
void AngleArrayData::setArray(const std::vector<MAngle>& array)
{
	size_t arrayLength = array.size();
	m_data.resize(arrayLength);

	for (unsigned int i = 0; i < arrayLength; i++)
		m_data[i] = array[i].asRadians();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------