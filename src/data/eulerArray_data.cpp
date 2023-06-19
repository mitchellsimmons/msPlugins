#include "eulerArray_data.h"
#include "utils/macros.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

EulerArrayData::EulerArrayData() : MRS::DataArrayHelper<MEulerRotation, double, 3>() {};

EulerArrayData::~EulerArrayData() {};

// ------ Const ------
// These must be defined outside of plugin.cpp, as the MTypeId is required by targets which link to the static data library
// Project Block0 IDs : [0x00130fc0 - 0x00130fcf]
const MTypeId EulerArrayData::kTypeId = 0x00130fc0;

const MString EulerArrayData::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "EulerArray";

// ------ MPxData ------
MTypeId EulerArrayData::typeId() const
{
	return EulerArrayData::kTypeId;
}

MString EulerArrayData::name() const
{
	return EulerArrayData::kTypeName;
}

// ------ MRS::DataArrayHelper ------
unsigned int EulerArrayData::length() const
{
	return MRS::DataArrayHelper<MEulerRotation, double, 3>::length() / 3;
}

// Used by get attribute methods
void EulerArrayData::getArray(std::vector<MEulerRotation>& outArray) const
{
	unsigned int arrayLength = length();
	outArray.resize(arrayLength);

	for (unsigned int i = 0; i < arrayLength; i++) {
		outArray[i] = MEulerRotation(m_data[(i * 3) + 0], m_data[(i * 3) + 1], m_data[(i * 3) + 2]);
	}
}

// Used by set attribute methods
void EulerArrayData::setArray(const std::vector<MEulerRotation>& array)
{
	size_t arrayLength = array.size();
	m_data.resize(arrayLength * 3);

	for (unsigned int i = 0; i < arrayLength; i++)
	{
		m_data[(i * 3) + 0] = array[i].x;
		m_data[(i * 3) + 1] = array[i].y;
		m_data[(i * 3) + 2] = array[i].z;
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------