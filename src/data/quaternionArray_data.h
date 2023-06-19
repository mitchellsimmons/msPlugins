#pragma once

#include <vector>

#include <maya/MQuaternion.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/data_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Custom DG data object used to read and write quaternion-array data
class QuaternionArrayData : public MRS::DataArrayHelper<MQuaternion, double, 4>
{
public:
	QuaternionArrayData();
	~QuaternionArrayData();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;
	
	// ------ MPxData ------
	MTypeId typeId() const override;
	MString name() const override;

	// ------ MRS::DataArrayHelper ------
	unsigned int length() const override;
	void getArray(std::vector<MQuaternion>& outArray) const override;
	void setArray(const std::vector<MQuaternion>& array) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------