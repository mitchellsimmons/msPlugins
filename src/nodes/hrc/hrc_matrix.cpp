#include "hrc_matrix.h"

/*	Description
	-----------
	This transformation matrix outputs the identity frame.
	It is designed to be used by custom MPxTransform nodes which provide non-transforming, hierarchical groupings within the DAG.
	These nodes will ignore any attempt at local transformation. */

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

HrcMatrix::HrcMatrix() : MPxTransformationMatrix() {};
HrcMatrix::~HrcMatrix() {};

// ------ MPxTransformationMatrix ------
MMatrix HrcMatrix::asMatrix() const
{
	return MMatrix::identity;
}

MMatrix HrcMatrix::asMatrix(double percent) const
{
	return MMatrix::identity;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------