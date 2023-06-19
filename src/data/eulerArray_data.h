#pragma once

#include <vector>

#include <maya/MEulerRotation.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/data_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Custom DG data object used to read and write euler-array data
class EulerArrayData : public MRS::DataArrayHelper<MEulerRotation, double, 3>
{
public:
	EulerArrayData();
	~EulerArrayData();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxData ------
	MTypeId typeId() const override;
	MString name() const override;

	// ------ MRS::DataArrayHelper ------
	unsigned int length() const override;
	void getArray(std::vector<MEulerRotation>& outArray) const override;
	void setArray(const std::vector<MEulerRotation>& array) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------