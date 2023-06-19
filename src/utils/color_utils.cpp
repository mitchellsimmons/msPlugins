#include "color_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Const ------
static const float _displayGamma = 2.2f;

// ------ Helpers ------

/*	Description
	-----------
	Approximately converts a color from the display color space (assumed to be sRGB) to a linear color space
	This is equivalent to applying the effect of a gamma decoding    */
MColor convertDisplayToLinear(const MColor& displayColor)
{
	float r,g,b,a;
	displayColor.get(MColor::kRGB, r, g, b, a);

	return MColor{ MColor::kRGB, std::pow(r, _displayGamma), pow(g, _displayGamma), pow(b, _displayGamma), a };
}

/*	Description
	-----------
	Approximately converts a color from the linear color space to the display color space (assumed to be sRGB)
	This is equivalent to applying the effect of a gamma encoding    */
MColor convertLinearToDisplay(const MColor& linearColor)
{
	float r, g, b, a;
	linearColor.get(MColor::kRGB, r, g, b, a);

	return MColor{ MColor::kRGB, std::pow(r, 1.0f / _displayGamma), pow(g, 1.0f / _displayGamma), pow(b, 1.0f / _displayGamma), a };
}

bool isColorManaged()
{
	return MColorManagementUtilities::isColorManagementEnabled();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------