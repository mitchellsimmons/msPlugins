#include "quaternionArray_data.h"
#include "utils/macros.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

QuaternionArrayData::QuaternionArrayData() : MRS::DataArrayHelper<MQuaternion, double, 4>() {};

QuaternionArrayData::~QuaternionArrayData() {};

// ------ Const ------
// These must be defined outside of plugin.cpp, as the MTypeId is required by targets which link to the static data library
// Project Block0 IDs : [0x00130fc0 - 0x00130fcf]
const MTypeId QuaternionArrayData::kTypeId = 0x00130fc1;

const MString QuaternionArrayData::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "QuaternionArray";

// ------ MPxData ------
MTypeId QuaternionArrayData::typeId() const
{
	return QuaternionArrayData::kTypeId;
}

MString QuaternionArrayData::name() const
{
	return QuaternionArrayData::kTypeName;
}

// ------ MRS::DataArrayHelper ------
unsigned int QuaternionArrayData::length() const
{
	return MRS::DataArrayHelper<MQuaternion, double, 4>::length() / 4;
}

// Used by get attribute methods
void QuaternionArrayData::getArray(std::vector<MQuaternion>& outArray) const
{
	unsigned int arrayLength = length();
	outArray.resize(arrayLength);

	for (unsigned int i = 0; i < arrayLength; i++) {
		outArray[i] = MQuaternion(m_data[(i * 4) + 0], m_data[(i * 4) + 1], m_data[(i * 4) + 2], m_data[(i * 4) + 3]);
	}
}

// Used by set attribute methods
void QuaternionArrayData::setArray(const std::vector<MQuaternion>& array)
{
	size_t arrayLength = array.size();
	m_data.resize(arrayLength * 4);

	for (unsigned int i = 0; i < arrayLength; i++)
	{
		m_data[(i * 4) + 0] = array[i].x;
		m_data[(i * 4) + 1] = array[i].y;
		m_data[(i * 4) + 2] = array[i].z;
		m_data[(i * 4) + 3] = array[i].w;
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------