#include "flexiChainTriple_locator.h"
#include "flexiChainTriple_locator_scaleAdjustment_manip.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiChainTriple_ScaleAdjustmentManip::FlexiChainTriple_ScaleAdjustmentManip() : MPxManipulatorNode(),
	m_locator{ nullptr },
	m_physicalIndex{ 0 },
	m_offsetHandleIndex{ 0 },
	m_activeName{ 0 },
	m_handleName{ 0 },
	m_pOffsetHandle{ 0.0f, 0.0f, 0.0f, 1.0f },
	m_isUserInteracting{ false },
	m_isMouseHovered{ false },
	m_isActive{ false }
{
	m_physicalIndex = physicalIndex;
}

FlexiChainTriple_ScaleAdjustmentManip::~FlexiChainTriple_ScaleAdjustmentManip()
{
	m_locator = nullptr;
}

// Static
unsigned int FlexiChainTriple_ScaleAdjustmentManip::physicalIndex = 0;
const MDistance FlexiChainTriple_ScaleAdjustmentManip::m_offsetHandleRadius{ 1.2, MDistance::kCentimeters };
const double FlexiChainTriple_ScaleAdjustmentManip::m_offsetHandleDragIncrement = 0.001;

// ------ MPxNode------

void FlexiChainTriple_ScaleAdjustmentManip::postConstructor()
{
	// Enable for hover state tracking
	registerForMouseMove();

	// Assign indices for selectable handles
	MGLuint glPickableItem;
	glFirstHandle(glPickableItem);
	m_handleName = glPickableItem;

	// Add values which will be connected to plugs
	addDoubleValue("curveParam", 0.0, m_offsetHandleIndex);
}

// ------ MPxManipulatorNode ------

MStatus FlexiChainTriple_ScaleAdjustmentManip::initialize()
{
	// No-op
	return MS::kSuccess;
}

/*	Description
	-----------
	This function updates the initial values of the manipulator
	It also creates mappings between manipulator values and their associated plugs

	Considerations
	--------------
	The mappings created by this function allow us to query the plug values using the get*Value() functions
	However, the set*Value() functions are causing Maya to crash so we will need to handle setting values via plugs    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::connectToDependNode(const MObject& node)
{
	MStatus status;

	MDagPath::getAPathTo(node, m_locatorPath);
	MFnDependencyNode fnDep{ node };
	m_locator = (FlexiChainTriple*)fnDep.userNode();

	// Offset Handle Mapping
	MPlug offsetPlug;
	status = getOffsetPlug(offsetPlug);
	if (status)
	{
		int plugIndex;
		connectPlugToValue(offsetPlug, m_offsetHandleIndex, plugIndex);
	}
	
	// Update initial values
	updateOffsetHandle();
	
	// Initialize converter
	finishAddingManips();
	MPxManipulatorNode::connectToDependNode(node);

	return MStatus::kSuccess;
}

void FlexiChainTriple_ScaleAdjustmentManip::preDrawUI(const M3dView& view)
{
	// The handle position resulting from user interaction is cached as a member variable but may need to be updated for drawing
	// If these values have been animated or the shape of the curve has changed as a result of animation, the data will be stale
	updateOffsetHandle();
	const FlexiChainTriple::FlexiChainTriple_Data& curveData = m_locator->getCurveData();
	m_pStart = curveData.points[0];
	m_pStart *= m_locatorPath.inclusiveMatrix();
	m_pEnd = curveData.points[curveData.sampleCount - 1];
	m_pEnd *= m_locatorPath.inclusiveMatrix();
}

/*	Description
	-----------
	Function is responsible for implementing the draw and selection interfaces    */
void FlexiChainTriple_ScaleAdjustmentManip::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const
{
	const short manipMainColor = mainColor();
	const short manipLineColor = lineColor();
	MPoint pCameraWorld = getCameraPosition(frameContext);
	
	MVector vAim = m_pOffsetHandle - pCameraWorld;
	vAim.normalize();
	MVector vUp = getCameraUpDirection(frameContext);
	MVector vRight = vAim ^ vUp;
	vRight.normalize();
	vUp = vRight ^ vAim;

	// Text
	MVector vCameraToDrawable = m_pOffsetHandle - pCameraWorld;
	double cameraToDrawableLength = vCameraToDrawable.length();
	drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
	MString indexText = std::to_string(m_physicalIndex).c_str();
	MPoint pText = m_pOffsetHandle + 2.0 * vUp;
	// We clamp the max font size to 16 when the distance is <= 2 meters
	MDistance refDist{ 200.0, MDistance::kCentimeters };
	double refDistInUI = refDist.asUnits(MDistance::uiUnit());
	// By default, the min font size of 1 would occur at 32 meters if we were to divide proportionally by the distance
	MDistance farDist{ 3200.0, MDistance::kCentimeters };
	double farDistInUI = farDist.asUnits(MDistance::uiUnit());
	// Ensure the distance at which a min occurs is clamped
	cameraToDrawableLength = std::max(refDistInUI, cameraToDrawableLength);
	// The following calculation remaps the distance so that the rate of change in font size will be three times slower than the default
	cameraToDrawableLength = refDistInUI + (cameraToDrawableLength - refDistInUI) * (farDistInUI/3 - refDistInUI) / (farDistInUI - refDistInUI);
	// Calculate the current font size relative to the remapped distance
	double fontSize = refDist.asUnits(MDistance::uiUnit()) * 16.0 / cameraToDrawableLength;
	// Ensure the max font size is clamped
	fontSize = std::min(fontSize, 16.0);
	// Apply a smoothing function to the result
	double fontWeight = fontSize / 16.0;
	double hermiteFontWeight = fontWeight * fontWeight * (3.0 - 2.0 * fontWeight);
	fontSize = 16.0 * hermiteFontWeight;
	// Ensure the min font size is clamped
	fontSize = std::max(1.0, fontSize);
	drawManager.setFontSize((int)fontSize);
	drawManager.setColor(MColor{ 1.0f, 1.0f, 1.0f });
	drawManager.text(pText, indexText, MHWRender::MUIDrawManager::TextAlignment::kCenter, nullptr, nullptr, false);
	drawManager.endDrawable();

	// Offset Handle
	vCameraToDrawable.normalize();
	drawManager.beginDrawable(MHWRender::MUIDrawManager::kSelectable, m_handleName);
	// Sets color used when the item is neither highlighted or selected
	setHandleColor(drawManager, m_handleName, manipMainColor);
	drawManager.setDepthPriority(MRenderItem::sSelectionDepthPriority);
	drawManager.circle(m_pOffsetHandle, vCameraToDrawable, m_offsetHandleRadius.asUnits(MDistance::uiUnit()), true);
	drawManager.endDrawable();

	// Overshoot Line
	if (m_offset < 0.0)
	{
		drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
		drawManager.setColorIndex(manipMainColor);
		drawManager.line(m_pStart, m_pOffsetHandle);
		drawManager.endDrawable();
	}
	else if (m_offset > 1.0)
	{
		drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
		drawManager.setColorIndex(manipMainColor);
		drawManager.line(m_pEnd, m_pOffsetHandle);
		drawManager.endDrawable();
	}
}

MStatus FlexiChainTriple_ScaleAdjustmentManip::doPress(M3dView& view)
{
	if (glActiveName(m_activeName))
	{
		if (m_activeName == m_handleName)
		{
			// Because of the way MPxSelectionContext handles selection, we are manually tracking which manipulator is visually selected
			// Essentially, a context with multiple containers can internally have multiple active manipulators (one per container)
			// However Maya will only display the last selected manipulator as being visually active
			MString activePlugCmd = "FlexiChainTripleScaleAdjustmentContext -e -activeManipPlug " + getCompoundPlugPartialName();
			activePlugCmd += " FlexiChainTripleScaleAdjustmentContext1";
			MGlobal::executeCommand(activePlugCmd);

			// Update the ramp UI if it exists and is visible (see MRS_FlexiChainTriple_scaleAdjustmentUIReplace.mel for implementation)
			// Because this command has the potential to modify the DG, it must be executed in an idle state
			// A ramp with no control points will force an element plug into existence, modifying the DG and causing Maya to crash if not in idle
			MString updateUICmd = "MRS_FlexiChainTriple_scaleAdjustmentUIReplace " + getCompoundPlugPartialName();
			MGlobal::executeCommandOnIdle(updateUICmd);

			MGlobal::executeCommand("undoInfo -openChunk");
			m_isUserInteracting = true;

			MPoint pCameraWorld = getCameraPosition(view);

			// Cache values which will be used to calculate selection offsets so that we can maintain the user's original mouse position relative to the selected item
			// Cache the original normal of the intersection plane for the current start point
			m_vInterPlaneNormalOffsetHandle = m_pOffsetHandle - pCameraWorld;
			m_vInterPlaneNormalOffsetHandle.normalize();
			// Cache the original position and direction of the mouse
			mouseRay(m_pMousePressOffsetHandle, m_vMousePressDirectionOffsetHandle);
		}
	}

	return MStatus::kSuccess;
}

/*	Description
	-----------
	Function finds the end position of the manipulator
	- The current mouse position is raycast onto three intersection planes all of which are normal to the camera
	- One intersection plane is centered at the current manipulator start point, the other two are positioned at small increments along the curve in either direction
	- An offset is calculated relative to each intersection plane by raycasting the original mouse down and manipulator start positions onto each plane
	- We compare the lengths of three vector going from the intersection on each plane to the respective center of each plane whilst maintaining the relative offset
	- The smallest vector corresponds to the closest point along the curve relative to the current mouse position and becomes our new start position for the manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::doDrag(M3dView& view)
{
	MStatus status;

	if (glActiveName(m_activeName))
	{
		if (m_activeName == m_handleName)
		{
			MPoint pCameraWorld = getCameraPosition(view);

			// Find the normal for each of the three intersection planes
			double offsetParam;
			getDoubleValue(m_offsetHandleIndex, false, offsetParam);
			MVector vInterPlaneNormalCurrent = m_pOffsetHandle - pCameraWorld;
			vInterPlaneNormalCurrent.normalize();

			double previousCurveParam = offsetParam - m_offsetHandleDragIncrement;
			MPoint pCurveSamplePrevious = sampleCurve(previousCurveParam);
			MVector vInterPlaneNormalPrevious = pCurveSamplePrevious - pCameraWorld;
			vInterPlaneNormalPrevious.normalize();

			double nextCurveParam = offsetParam + m_offsetHandleDragIncrement;
			MPoint pCurveSampleNext = sampleCurve(nextCurveParam);
			MVector vInterPlaneNormalNext = pCurveSampleNext - pCameraWorld;
			vInterPlaneNormalNext.normalize();

			// Find the intersections of the current mouse position raycast onto each intersection plane
			MPoint pInterMouseEndCurrent;
			MPoint pInterMouseEndPrevious;
			MPoint pInterMouseEndNext;
			computeCurrentMouseIntersection(m_pOffsetHandle, vInterPlaneNormalCurrent, pInterMouseEndCurrent);
			computeCurrentMouseIntersection(pCurveSamplePrevious, vInterPlaneNormalPrevious, pInterMouseEndPrevious);
			computeCurrentMouseIntersection(pCurveSampleNext, vInterPlaneNormalNext, pInterMouseEndNext);

			// Find the intersections of the original mouse press position raycast onto each intersection plane
			MPoint pInterOriginalMousePressCurrent;
			MPoint pInterOriginalMousePressPrevious;
			MPoint pInterOriginalMousePressNext;
			computeMouseIntersection(m_pMousePressOffsetHandle, m_vMousePressDirectionOffsetHandle, m_pOffsetHandle, vInterPlaneNormalCurrent, pInterOriginalMousePressCurrent);
			computeMouseIntersection(m_pMousePressOffsetHandle, m_vMousePressDirectionOffsetHandle, pCurveSamplePrevious, vInterPlaneNormalPrevious, pInterOriginalMousePressPrevious);
			computeMouseIntersection(m_pMousePressOffsetHandle, m_vMousePressDirectionOffsetHandle, pCurveSampleNext, vInterPlaneNormalNext, pInterOriginalMousePressNext);
			// Find the intersection of the original handle position raycast onto each intersection plane
			MPoint pInterOriginalHandleCurrent;
			MPoint pInterOriginalHandlePrevious;
			MPoint pInterOriginalHandleNext;
			computeMouseIntersection(pCameraWorld, m_vInterPlaneNormalOffsetHandle, m_pOffsetHandle, vInterPlaneNormalCurrent, pInterOriginalHandleCurrent);
			computeMouseIntersection(pCameraWorld, m_vInterPlaneNormalOffsetHandle, pCurveSamplePrevious, vInterPlaneNormalPrevious, pInterOriginalHandlePrevious);
			computeMouseIntersection(pCameraWorld, m_vInterPlaneNormalOffsetHandle, pCurveSampleNext, vInterPlaneNormalNext, pInterOriginalHandleNext);
			// Find the relative offset between the two intersection points on each intersection plane
			MVector vMousePressOffsetCurrent = pInterOriginalMousePressCurrent - pInterOriginalHandleCurrent;
			MVector vMousePressOffsetPrevious = pInterOriginalMousePressPrevious - pInterOriginalHandlePrevious;
			MVector vMousePressOffsetNext = pInterOriginalMousePressNext - pInterOriginalHandleNext;

			// Calculate an offset for each mouse intersection to the respective sample point (center of intersection plane)
			// This offset must take into account the inital mouse press offset which has been raycast and calculated relative to each intersection plane
			MVector vMouseOffsetCurrent = pInterMouseEndCurrent - vMousePressOffsetCurrent - m_pOffsetHandle;
			MVector vMouseOffsetPrevious = pInterMouseEndPrevious - vMousePressOffsetPrevious - pCurveSamplePrevious;
			MVector vMouseOffsetNext = pInterMouseEndNext - vMousePressOffsetNext - pCurveSampleNext;

			// Update the start position if necessary
			double currentOffsetLength = vMouseOffsetCurrent.length();
			double previousOffsetLength = vMouseOffsetPrevious.length();
			double nextOffsetLength = vMouseOffsetNext.length();

			if (previousOffsetLength < currentOffsetLength)
			{
				m_pOffsetHandle = pCurveSamplePrevious;
				status = updateOffsetHandlePlug(previousCurveParam);

				// The refresh rate of this function is not high enough for our increment size (which is small to ensure smooth movement), this creates a lag effect
				// Recursively call the function until the offset between the mouse and the current sample point is smaller than both the previous and next offset
				// The offset lengths are affected by the normal of each intersection plane which are continually changing during recursion
				// Because of this, it is possible that the difference in length between two recursive calls is so small that the direction changes
				// If this occurs, the result is a recursive loop, therefore we must ensure the direction only propagates in a single direction during recursion
				if (m_offsetHandleDragDirection == 0 || m_offsetHandleDragDirection == -1)
				{
					m_offsetHandleDragDirection = -1;
					doDrag(view);
				}
			}
			else if (nextOffsetLength < currentOffsetLength)
			{
				m_pOffsetHandle = pCurveSampleNext;
				status = updateOffsetHandlePlug(nextCurveParam);

				if (m_offsetHandleDragDirection == 0 || m_offsetHandleDragDirection == 1)
				{
					m_offsetHandleDragDirection = 1;
					doDrag(view);
				}
			}

			m_offsetHandleDragDirection = 0;
		}
	}

	return status;
}

MStatus FlexiChainTriple_ScaleAdjustmentManip::doRelease(M3dView& view)
{
	if (m_isUserInteracting)
	{
		MGlobal::executeCommand("undoInfo -closeChunk");
		m_isUserInteracting = false;
	}
	
	return MStatus::kSuccess;
}

/*	Description
	-----------
	This function tracks whether the current mouse position is hovering over the manipulator handle
	When the user right clicks on the handle, a callback is invoked which checks the hover state of each manipulator in the active context
	If the hover state is active, the callback will create a marking menu for that specific manipulator
	The menu allows the user to execute certain operations on the manipulator and its associated plug    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::doMove(M3dView& view, bool& refresh)
{
	MPoint pCameraWorld = getCameraPosition(view);

	MVector vInterPlaneNormalHandle = m_pOffsetHandle - pCameraWorld;
	vInterPlaneNormalHandle.normalize();

	// Calculate the mouse intersection on the normal plane
	MPoint pInterMouseHandle;
	computeCurrentMouseIntersection(m_pOffsetHandle, vInterPlaneNormalHandle, pInterMouseHandle);

	// Find the offset from the center of the plane
	MVector vMouseOffsetHandle = m_pOffsetHandle - pInterMouseHandle;

	// If the offset is smaller than the handle's radius then the mouse is within the hover region
	// The MUIDrawManager adds a hover buffer region around items which appears to be proportional to the items' distance from the camera
	// The below calculation attempts to mimic this hover buffer region with fairly high accuracy (not actually sure how it works internally)
	MPoint pCamera = getCameraPosition(view);
	MVector vCameraToHandle = m_pOffsetHandle - pCamera;
	// The max multiplier of 4.0 occurs at 10 meters
	MDistance refDist{ 1000, MDistance::kCentimeters };
	double hoverRadiusMult = vCameraToHandle.length() / refDist.asUnits(MDistance::uiUnit()) * 4.0;
	// Clamp the multiplier to the max (ie. past 10 meters the multiplier is always 4.0)
	hoverRadiusMult = std::min(hoverRadiusMult, 4.0);
	// Remap the multiplier so the low is always 1.0
	// Remap = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
	hoverRadiusMult = 1.0 + hoverRadiusMult * 3.0 / 4.0;

	if (vMouseOffsetHandle.length() <= m_offsetHandleRadius.asUnits(MDistance::uiUnit()) * hoverRadiusMult)
		m_isMouseHovered = true;
	else
		m_isMouseHovered = false;

	return MStatus::kSuccess;
}

// ------ Helpers ------

/*	Description
	-----------
	Function retrieves the compound element plug associated with this manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::getCompoundPlug(MPlug& compoundPlug)
{
	MStatus status;

	MPlug compoundArrayPlug{ m_locatorPath.node(), FlexiChainTriple::scaleAdjustmentCompoundAttr };
	compoundPlug = compoundArrayPlug.elementByPhysicalIndex(m_physicalIndex, &status);

	return status;
}

/*	Description
	-----------
	Function retrieves the ramp plug associated with this manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::getRampCompoundPlug(MPlug& rampPlug)
{
	MStatus status;

	MPlug compoundPlug;
	status = getCompoundPlug(compoundPlug);
	if (status)
		rampPlug = compoundPlug.child(FlexiChainTriple::scaleAdjustmentRampAttr, &status);

	return status;
}

/*	Description
	-----------
	Function retrieves the scale plug associated with this manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::getScalePlug(MPlug& scalePlug)
{
	MStatus status;

	MPlug compoundPlug;
	status = getCompoundPlug(compoundPlug);
	if (status)
		scalePlug = compoundPlug.child(FlexiChainTriple::scaleAdjustmentValueAttr, &status);

	return status;
}

/*	Description
	-----------
	Function retrieves the offset plug associated with this manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::getOffsetPlug(MPlug& offsetPlug)
{
	MStatus status;

	MPlug compoundPlug;
	status = getCompoundPlug(compoundPlug);
	if (status)
		offsetPlug = compoundPlug.child(FlexiChainTriple::scaleAdjustmentOffsetAttr, &status);

	return status;
}

/*	Description
	-----------
	Function returns the partial node.plug name of the compound element plug associated with this manipulator (ensuring both node and plug are partial)    */
MString FlexiChainTriple_ScaleAdjustmentManip::getCompoundPlugPartialName()
{
	MString locatorName = m_locatorPath.partialPathName();
	MPlug compoundPlug;
	MStatus status = getCompoundPlug(compoundPlug);
	return status ? locatorName + "." + compoundPlug.partialName() : "";
}

bool FlexiChainTriple_ScaleAdjustmentManip::isUserInteracting()
{
	return m_isUserInteracting;
}

bool FlexiChainTriple_ScaleAdjustmentManip::isMouseHovered()
{
	return m_isMouseHovered;
}

bool FlexiChainTriple_ScaleAdjustmentManip::isActive()
{
	return m_isActive;
}

/*	Description
	-----------
	Samples the curve with respect to the current parameterization blend, using the given parameter space position
	- If the curve is parameterized in terms of its arc-length, the given parameter will be interpreted as a normalized arc-length value
	- If the curve is parameterized in terms of its natural parameter, the given parameter will be assumed to be representative of this

	Unlike the standard sampling function, this will allow a parameter outside of the defined range [0-1]
	- A parameter < 0.0 will produce a point which lies along the initial tangent and whose distance from the initial point is proportional to the curves length
	- A parameter > 1.0 will produce a point which lies along the end tangent and whose distance from the end point is proportional to the curves length

	The point will be transformed into a world space coordinate    */
MPoint FlexiChainTriple_ScaleAdjustmentManip::sampleCurve(double param)
{
	MPoint pPosition;

	const FlexiChainTriple::FlexiChainTriple_Data& curveData = m_locator->getCurveData();
	double arcLength = curveData.lengths[curveData.parameterCount - 1];

	if (param < 0.0)
	{
		MVector vLowerBoundTangent = curveData.controlPoints0[1] - curveData.controlPoints0[0];
		vLowerBoundTangent.normalize();
		pPosition = (curveData.points[0] + param * arcLength * vLowerBoundTangent) * m_locatorPath.inclusiveMatrix();
	}
	else if (param > 1.0)
	{
		MVector vUpperBoundTangent = curveData.controlPoints2[3] - curveData.controlPoints2[2];
		vUpperBoundTangent.normalize();
		pPosition = (curveData.points[curveData.sampleCount - 1] + (param - 1.0) * arcLength * vUpperBoundTangent) * m_locatorPath.inclusiveMatrix();
	}
	else
	{
		double currentParam;
		if (curveData.parameterization == 0)
			currentParam = param * 3.0;
		else if (curveData.parameterization == 1)
			// Calculate the natural parameter at the given normalized arc-length
			currentParam = m_locator->getCurve().arcLengthToNaturalParameter(param, curveData.lengths) * 3.0;
		else
			// Calculate the natural parameter at the given normalized split-length
			currentParam = m_locator->splitLengthToNaturalParameter(param * 3.0);

		pPosition = m_locator->sampleCurve(currentParam);
		pPosition *= m_locatorPath.inclusiveMatrix();
	}

	return pPosition;
}

// Legacy implementation
MPoint FlexiChainTriple_ScaleAdjustmentManip::getCameraPosition(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MPoint pCameraWorld = MFnTransform(cameraPath).rotatePivot(MSpace::kWorld);
	return pCameraWorld;
}

// VP2 implementation
MPoint FlexiChainTriple_ScaleAdjustmentManip::getCameraPosition(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray viewPosition = frameContext.getTuple(MHWRender::MFrameContext::kViewPosition);
	MPoint pCameraWorld{ viewPosition[0], viewPosition[1], viewPosition[2] };
	return pCameraWorld;
}

// Legacy implementation
MVector FlexiChainTriple_ScaleAdjustmentManip::getCameraAimDirection(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MFnCamera cameraFn{ cameraPath };
	MVector vAimDirection = cameraFn.viewDirection(MSpace::kWorld);
	return vAimDirection;
}

// VP2 implementation
MVector FlexiChainTriple_ScaleAdjustmentManip::getCameraAimDirection(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray aimDirection = frameContext.getTuple(MHWRender::MFrameContext::kViewDirection);
	MPoint vAimDirection{ aimDirection[0], aimDirection[1], aimDirection[2] };
	return vAimDirection;
}

// Legacy implementation
MVector FlexiChainTriple_ScaleAdjustmentManip::getCameraUpDirection(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MFnCamera cameraFn{ cameraPath };
	MVector vUpDirection = cameraFn.upDirection(MSpace::kWorld);
	return vUpDirection;
}

// VP2 implementation
MVector FlexiChainTriple_ScaleAdjustmentManip::getCameraUpDirection(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray upDirection = frameContext.getTuple(MHWRender::MFrameContext::kViewUp);
	MPoint vUpDirection{ upDirection[0], upDirection[1], upDirection[2] };
	return vUpDirection;
}

/*	Description
	-----------
	Calculates the intersection of the given mouse position and direction raycast onto the plane determined by the given plane data
	Returns true if the raycast produces a successful intersection    */
bool FlexiChainTriple_ScaleAdjustmentManip::computeMouseIntersection(const MPoint& pMouse, const MVector& vMouseDirection, const MPoint& pPointOnInterPlane,
	const MVector& vInterPlaneNormal, MPoint& pIntersection)
{
	// Vector from the mouse to a point on the intersection plane
	MVector vMouseToPointOnInterPlane = pPointOnInterPlane - pMouse;

	// Here we are finding the ratio between two projections
	// - The above vector projected onto the intersection plane normal
	// - The mouse direction projected onto the intersection plane normal
	double projectionRatio = (vMouseToPointOnInterPlane * vInterPlaneNormal) / (vMouseDirection * vInterPlaneNormal);

	// Scale the mouse direction vector by the ratio then add to the mouse position to get the intersection point
	MVector vMouseToInterPoint = vMouseDirection * projectionRatio;
	pIntersection = pMouse + vMouseToInterPoint;

	// Return true if both projections are in the same direction
	return projectionRatio >= 0.0;
}

/*	Description
	-----------
	Calculates the intersection of the current mouse position raycast onto the plane determined by the given plane data
	Returns true if the raycast produces a successful intersection    */
bool FlexiChainTriple_ScaleAdjustmentManip::computeCurrentMouseIntersection(const MPoint& pPointOnInterPlane, const MVector& vInterPlaneNormal, MPoint& pIntersection)
{
	// Find the mouse point and direction in world space (the mouse point essentially lies on the camera lens)
	MPoint pMouse;
	MVector vMouseDirection;
	if (!mouseRay(pMouse, vMouseDirection))
		return false;

	MVector vMouseToPointOnInterPlane = pPointOnInterPlane - pMouse;
	double projectionRatio = (vMouseToPointOnInterPlane * vInterPlaneNormal) / (vMouseDirection * vInterPlaneNormal);
	MVector vMouseToInterPoint = vMouseDirection * projectionRatio;
	pIntersection = pMouse + vMouseToInterPoint;

	return projectionRatio >= 0.0;
}

/*	Description
	-----------
	Updates the cached offset handle position so that the handle can be drawn correctly
	Usually called during setup or when the shape of the curve changes whilst the context is active    */
void FlexiChainTriple_ScaleAdjustmentManip::updateOffsetHandle()
{
	getDoubleValue(m_offsetHandleIndex, false, m_offset);
	m_pOffsetHandle = sampleCurve((double)m_offset);
}

/*	Description
	-----------
	This method exists to resolve crashing issues caused when using setVectorValue methods
	It updates the offset child plug for the compound element plug associated with this manipulator    */
MStatus FlexiChainTriple_ScaleAdjustmentManip::updateOffsetHandlePlug(double offset)
{
	MStatus status;

	MPlug offsetPlug;
	status = getOffsetPlug(offsetPlug);
	if (status)
	{		
		// Ownership will be transferred to the command object
		MDGModifier* dgMod = new MDGModifier();
		dgMod->newPlugValueDouble(offsetPlug, offset);
		status = MRS::executeUndoModifierCmd(dgMod);
	}

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiChainTriple_ScaleAdjustmentManipContainer::FlexiChainTriple_ScaleAdjustmentManipContainer() : MPxManipContainer(),
	m_manip{ nullptr },
	m_locator{ nullptr }
{}

FlexiChainTriple_ScaleAdjustmentManipContainer::~FlexiChainTriple_ScaleAdjustmentManipContainer()
{
	m_manip = nullptr;
	m_locator = nullptr;
}

// ------ MPxManipContainer ------

MStatus FlexiChainTriple_ScaleAdjustmentManipContainer::initialize()
{
	return MPxManipContainer::initialize();
}

MStatus FlexiChainTriple_ScaleAdjustmentManipContainer::createChildren()
{
	MStatus status;
	
	MPxManipulatorNode *proxyManip = 0;
	MString manipTypeName(FlexiChainTriple_ScaleAdjustmentManip::kTypeName);
	MString manipName("FlexiChainTripleScaleAdjustmentManipInst");
	status = addMPxManipulatorNode(manipTypeName, manipName, proxyManip);

	if (status)
		m_manip = (FlexiChainTriple_ScaleAdjustmentManip*)proxyManip;

	return status;
}

MStatus FlexiChainTriple_ScaleAdjustmentManipContainer::connectToDependNode(const MObject& node)
{
	MStatus status;

	MDagPath::getAPathTo(node, m_locatorPath);
	MFnDependencyNode fnDep{ node };
	m_locator = (FlexiChainTriple*)fnDep.userNode();
	m_manip->connectToDependNode(node);

	// Initialize converter
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiChainTriple_ScaleAdjustmentContext::FlexiChainTriple_ScaleAdjustmentContext() : MPxSelectionContext(),
	m_numActiveNodes{ 0 },
	m_selectionChangedCallbackId{ 0 }
{
	MString titleStr("FlexiChainTriple Scale Adjustment Tool");
	setTitleString(titleStr);
}

FlexiChainTriple_ScaleAdjustmentContext::~FlexiChainTriple_ScaleAdjustmentContext() {}

// ------ MPxSelectionContext ------

/*	Description
	-----------
	This method is called when the context is activated
	It instantiates manipulators for the currently hilited FlexiChainTriple nodes
	It adds a callback which is invoked when the selection changes so that manipulators can be reinstantiated
	It adds a callback for each hilited node, responsible for tracking attribute changes and updating the associated manipulator values    */
void FlexiChainTriple_ScaleAdjustmentContext::toolOnSetup(MEvent& event)
{
	MStatus status;

	// Enter component mode but disable selection masks for components (we are just using component mode as a visual indicator)
	MGlobal::executeCommand("selectMode -component");
	MGlobal::executeCommand("selectType -allComponents false");

	MString helpStr("Drag manipulators to define the offset for each scale adjustment falloff. Right click for more options.");
	setHelpString(helpStr);

	// Add a callback for selection changes
	m_selectionChangedCallbackId = MEventMessage::addEventCallback("SelectionChanged", updateManipulatorsCallback, this, &status);
	if (!status)
		std::cerr << "FlexiChainTriple_ScaleAdjustmentContext: MEventMessage::addEventCallback() failed" << std::endl;

	// Update manipulators
	updateManipulatorsCallback(this);
}

/*	Description
	-----------
	This method is called if an instance of this context is active when the setToolTo command is executed
	Even if the context being set is already active, this function will still be invoked, effectively refreshing the context
	It undoes the work of the setup function, removing all MPxManipContainers and all callbacks which are owned by the current instance of this context     */
void FlexiChainTriple_ScaleAdjustmentContext::toolOffCleanup()
{
	MStatus status;

	MGlobal::executeCommand("selectMode -object");
	MGlobal::executeCommand("selectType -allComponents true");
	// After the context is deactivated, text from previous drawFeedback() calls does not update until the user moves the camera, force this update
	MGlobal::executeCommandOnIdle("refresh");

	// Remove callbacks and reset variables
	status = MNodeMessage::removeCallback(m_selectionChangedCallbackId);
	if (!status)
		cerr << "FlexiChainTriple_ScaleAdjustmentContext: MNodeMessage::removeCallback() failed" << std::endl;
	m_selectionChangedCallbackId = 0;

	for (MCallbackId& id : m_nodePlugChangedCallbackIds)
	{
		status = MNodeMessage::removeCallback(id);
		if (!status)
			cerr << "FlexiChainTriple_ScaleAdjustmentContext: MNodeMessage::removeCallback() failed" << std::endl;
	}
	m_nodePlugChangedCallbackIds.clear();

	// Delete manipulators
	m_manipContainers.clear();
	deleteManipulators();

	MPxContext::toolOffCleanup();
}

/*	Description
	-----------
	Prints the tool description in the help line     */
MStatus FlexiChainTriple_ScaleAdjustmentContext::doEnterRegion(MEvent& event)
{
	MString helpStr("Drag manipulators to define the offset for each scale adjustment falloff. Right click for more options.");
	return setHelpString(helpStr);
}

/*	Description
	-----------
	Draws contextual information about the current state of the context     */
MStatus FlexiChainTriple_ScaleAdjustmentContext::drawFeedback(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext)
{
	int viewportOriginX, viewportOriginY, viewportWidth, viewportHeight;
	frameContext.getViewportDimensions(viewportOriginX, viewportOriginY, viewportWidth, viewportHeight);
	MPoint pText{ (double)viewportWidth / 2, (double)viewportHeight * 0.97, 0 };

	drawManager.setFontSize(13);
	MString activeItems = std::to_string(m_numActiveNodes).c_str();
	MString text;
	if (m_numActiveNodes == 1)
		text = ("FlexiChainTriple  :  Scale Adjustment Tool  :  " + activeItems) + " Active Node";
	else
		text = ("FlexiChainTriple  :  Scale Adjustment Tool  :  " + activeItems) + " Active Nodes";
	drawManager.text2d(pText, text, MHWRender::MUIDrawManager::TextAlignment::kCenter, nullptr, nullptr, false);
	drawManager.endDrawable();

	return MStatus::kSuccess;
}

// ------ Helpers ------

/*	Description
	-----------
	This function is invoked by our callback which responds to RMB events on FlexiChainTriple nodes
	If the mouse is currently hovered over a manipulator which belongs to this context, it will return the partial name of the plug to which the manipulator is associated
	If the mouse is not hovered over a manipulator, an empty string will be returned    */
MString FlexiChainTriple_ScaleAdjustmentContext::getHoverManipPlug()
{
	for (FlexiChainTriple_ScaleAdjustmentManipContainer*& manipContainer : m_manipContainers)
	{
		if (manipContainer->m_manip->isMouseHovered())
			return manipContainer->m_manip->getCompoundPlugPartialName();
	}

	return MString();
}

/*	Description
	-----------
	This function is invoked by the associated MPxSubSceneOverride registered to FlexiChainTriple nodes
	If a manipulator which belongs to this context is currently active, it will return the partial name of the plug to which the manipulator is associated
	If no manipulator is active, an empty string will be returned    */
MString FlexiChainTriple_ScaleAdjustmentContext::getActiveManipPlug()
{
	for (FlexiChainTriple_ScaleAdjustmentManipContainer*& manipContainer : m_manipContainers)
	{
		if (manipContainer->m_manip->isActive())
			return manipContainer->m_manip->getCompoundPlugPartialName();
	}

	return MString();
}

/*	Description
	-----------
	This function is invoked when a manipulator belonging to this context receives a mouse press from the user
	The compound plug name associated with the pressed manipulator will be passed to this function
	The active state tracker of the pressed manipulator will be enabled, all other manipulator active state trackers will be disabled
	The active state of manipulators within the active context is manually tracked as Maya internally tracks active manipulators per container, not over the entire context    */
void FlexiChainTriple_ScaleAdjustmentContext::setActiveManipPlug(MString& activeManipPlugName)
{
	MStatus status;

	MSelectionList selList;
	selList.add(activeManipPlugName);
	MPlug activeManipPlug;
	status = selList.getPlug(0, activeManipPlug);

	if (status)
	{
		for (FlexiChainTriple_ScaleAdjustmentManipContainer*& manipContainer : m_manipContainers)
		{
			MPlug manipPlug;
			status = manipContainer->m_manip->getCompoundPlug(manipPlug);

			if (status && manipPlug.logicalIndex() == activeManipPlug.logicalIndex())
				manipContainer->m_manip->m_isActive = true;
			else
				manipContainer->m_manip->m_isActive = false;
		}
	}
}

/*	Description
	-----------
	This function is responsible for setting up manipulators and their associated callbacks
	It is invoked as the result of two possible actions, activating the tool and changing selection whilst the tool context is active    */
void FlexiChainTriple_ScaleAdjustmentContext::updateManipulatorsCallback(void* data)
{
	MStatus status;

	FlexiChainTriple_ScaleAdjustmentContext* context = (FlexiChainTriple_ScaleAdjustmentContext*)data;
	context->m_numActiveNodes = 0;

	// Remove callbacks for when selection has changed
	for (MCallbackId& id : context->m_nodePlugChangedCallbackIds)
	{
		status = MNodeMessage::removeCallback(id);
		if (!status)
			cerr << "FlexiChainTriple_ScaleAdjustmentContext: MNodeMessage::removeCallback() failed" << std::endl;
	}
	context->m_nodePlugChangedCallbackIds.clear();

	// Delete manipulators
	context->m_manipContainers.clear();
	context->deleteManipulators();

	// When the tool is activated, component selection mode is enabled which removes nodes from the active selection list and adds them to the hilite list
	// Create manipulators for each FlexiChainTriple object that is currently in hilite display mode and store them with the context so we can delete them later
	MSelectionList hiliteList;
	MGlobal::getHiliteList(hiliteList);
	// The node type filter will cause the shape to be iterated if the transform of a custom plugin shape is selected
	// If the shape and the transform are selected, the shape will appear in the list multiple times
	MItSelectionList iter{ hiliteList, MFn::kPluginDependNode };
	MObjectArray seenNodes;

	while (!iter.isDone())
	{
		MObject nodeObj = MObject::kNullObj;
		iter.getDependNode(nodeObj);
		MFnDependencyNode fnDep{ nodeObj };

		bool seen = false;
		for (const MObject& seenNode : seenNodes)
		{
			if (seenNode == nodeObj)
				seen = true;
		}

		if (!seen && fnDep.typeId() == FlexiChainTriple::kTypeId)
		{
			seenNodes.append(nodeObj);
			++context->m_numActiveNodes;

			// We want to ensure that the locator holds the correct adjustment data so that it can be accessed by the sub-scene override
			// Because compute is not triggered when the number of elements held by an array plug is altered, this data may be stale
			// However this callback will always be triggered when the array plug is altered, therefore we can ensure this data will remain updated
			FlexiChainTriple* locator = (FlexiChainTriple*)fnDep.userNode();
			MDataBlock dataBlock = locator->getDataBlock();
			const FlexiChainTriple::FlexiChainTriple_Data& curveData = locator->getCurveData();
			if (curveData.isScaleAdjustmentEnabled)
				locator->computeScaleAdjustments(dataBlock);

			// MPxManipContainer::createChildren() will be called directly after each new manipulator container is instantiated below
			// The static member "physicalIndex" holds transient data as a way of passing it to the constructor so it can be set as a member
			MPlug scaleAdjustmentPlug = fnDep.findPlug("scaleAdjustment", true, &status);
			for (unsigned int i = 0; i < scaleAdjustmentPlug.evaluateNumElements(); ++i)
			{
				FlexiChainTriple_ScaleAdjustmentManip::physicalIndex = i;
				MString manipName(FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeName);
				MObject manipObject = MObject::kNullObj;
				FlexiChainTriple_ScaleAdjustmentManipContainer* manipContainer = (FlexiChainTriple_ScaleAdjustmentManipContainer*)FlexiChainTriple_ScaleAdjustmentManipContainer::newManipulator(manipName, manipObject);

				if (manipContainer)
				{
					context->addManipulator(manipObject);
					context->m_manipContainers.push_back(manipContainer);
					manipContainer->connectToDependNode(nodeObj);
				}
			}

			// MPxManipulatorNode does not provide callbacks as part of its interface for setting the manipulator values when the associated plug values change
			// Set up a callback for each node which will track when evaluation of an output attribute has occurred
			MCallbackId nodePlugChangedId = MNodeMessage::addAttributeChangedCallback(nodeObj, nodePlugChangedCallback, context, &status);
			if (status)
				context->m_nodePlugChangedCallbackIds.push_back(nodePlugChangedId);
			else
				std::cerr << "FlexiChainTriple_ScaleAdjustmentContext: MNodeMessage::addAttributeChangedCallback() failed" << std::endl;
		}

		iter.next();
	}
}

/*	Description
	-----------
	This callback is registered to each node which is tied to the current context
	It tracks when an output attribute has been evaluated and updates manipulator values as a result
	
	Considerations
	--------------
	We do not want to exclusively track when the input attributes affecting manipulators change
	- Consider a change which affects the overall shape of the curve, this may not affect the local parameter of the manipulator along the curve
	- However it should affect its global position and therefore an update will be required
	Therefore instead of tracking changes to input attributes, we are tracking when evaluation has occurred
	- A consequence of this is that this function will also be invoked during user interaction (eg. drag operations) as these operations update plug values
	- An optimisation is made so that invocations of this function are ignored during user interaction, as the interaction function is responsible for the update    */
void FlexiChainTriple_ScaleAdjustmentContext::nodePlugChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* data)
{
	// Note, even though the drag interaction is completed in a single undo chunk, undoing the operation will invoke this callback for each command in the chunk
	// Wait for the below operation to complete and for an output to evaluate before updating manipulators
	if (MGlobal::isUndoing() || MGlobal::isRedoing())
		return;
	
	if (msg & MNodeMessage::AttributeMessage::kAttributeEval)
	{
		if (plug == FlexiChainTriple::outputLocalFramesAttr || plug == FlexiChainTriple::outputLocalPositionsAttr ||
			plug == FlexiChainTriple::outputLocalOrientationsAttr || plug == FlexiChainTriple::outputWorldFramesAttr ||
			plug == FlexiChainTriple::outputWorldPositionsAttr || plug == FlexiChainTriple::outputWorldOrientationsAttr)
		{
			// This command will have the same effect as the below solution, however it resets the current handle selection and is less efficient
			//MGlobal::executeCommandOnIdle("setToolTo `currentCtx`");

			FlexiChainTriple_ScaleAdjustmentContext* context = (FlexiChainTriple_ScaleAdjustmentContext*)data;
			for (FlexiChainTriple_ScaleAdjustmentManipContainer*& manipContainer : context->m_manipContainers)
			{
				if (!manipContainer->m_manip->isUserInteracting())
				{
					manipContainer->m_manip->updateOffsetHandle();
				}
			}
		}
	}
	else if (msg & MNodeMessage::AttributeMessage::kAttributeArrayAdded || msg & msg & MNodeMessage::AttributeMessage::kAttributeArrayRemoved)
		// This must not be on idle, otherwise the existing manipulator will access the plug causing it to be recreated before the manipulator can be deleted
		MGlobal::executeCommand("setToolTo `currentCtx`");
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The FlexiChainTripleScaleAdjustmentContext command is used to generate a custom context which can then be used to modify scale adjustments
	- A manipulator is created for each scaleAdjustment element plug, on each selected FlexiChainTriple node assigned to the active context
	- This command allows the user to query certain states relating to those manipulators

	MEL Command
	-----------
	FlexiChainTripleScaleAdjustmentContext [-hoverManipPlug bool] [-activeManipPlug bool] [context]

	Flags
	-----
	-hoverManipPlug(-hmp)
		This flag is used to query the currently hovered manipulator handle within the active context, specified using the context argument
		- If the user's mouse is hovered over a manipulator handle, the name of the compound element plug associated with that manipulator will be returned
		- If no manipulator handle is currently hovered, an empty string will be returned

	-activeManipPlug(-amp)
		This flag is used to query the currently active manipulator handle within the active context, specified using the context argument
		The active handle is being considered as the one which is visually selected (internally Maya allows an active handle per MPxManipContainer)
		- If a manipulator handle within the given context is active, the name of the compound element plug associated with that manipulator will be returned
		- If no manipulator handle is currently active, an empty string will be returned

	Args
	----
	context
		This is the name given to the FlexiChainTripleScaleAdjustmentContext instance upon creation
		When querying the active context, this name must be passed to the command
*/

FlexiChainTriple_ScaleAdjustmentContextCommand::FlexiChainTriple_ScaleAdjustmentContextCommand() : MPxContextCommand(),
	m_context{ nullptr }
{}

FlexiChainTriple_ScaleAdjustmentContextCommand::~FlexiChainTriple_ScaleAdjustmentContextCommand()
{
	m_context = nullptr;
}

// ------ MPxContextCommand ------

#define kErrorInvalidFlagCombination \
	"Only one of -hoverManipPlug/-activeManipPlug flags may be specified."

const char* FlexiChainTriple_ScaleAdjustmentContextCommand::kHoverFlag = "-hmp";
const char* FlexiChainTriple_ScaleAdjustmentContextCommand::kHoverFlagLong = "-hoverManipPlug";
const char* FlexiChainTriple_ScaleAdjustmentContextCommand::kActiveFlag = "-amp";
const char* FlexiChainTriple_ScaleAdjustmentContextCommand::kActiveFlagLong = "-activeManipPlug";

MPxContext* FlexiChainTriple_ScaleAdjustmentContextCommand::makeObj()
{
	m_context = new FlexiChainTriple_ScaleAdjustmentContext;
	return m_context;
}

MStatus FlexiChainTriple_ScaleAdjustmentContextCommand::doEditFlags()
{
	MStatus status;
	MArgParser argParser = parser();

	// Validate flags
	if (argParser.isFlagSet(kHoverFlagLong) && argParser.isFlagSet(kActiveFlagLong))
	{
		MGlobal::displayError(kErrorInvalidFlagCombination);
		return MStatus::kFailure;
	}

	// Parse values
	if (argParser.isFlagSet(kActiveFlagLong))
	{
		MString activePlugName = argParser.flagArgumentString(kActiveFlagLong, 0, &status);
		m_context->setActiveManipPlug(activePlugName);
	}

	return status;
}

MStatus FlexiChainTriple_ScaleAdjustmentContextCommand::doQueryFlags()
{
	MStatus status;
	MArgParser argParser = parser();
	
	// Validate flags
	if (argParser.isFlagSet(kHoverFlagLong) && argParser.isFlagSet(kActiveFlagLong))
	{
		MGlobal::displayError(kErrorInvalidFlagCombination);
		return MStatus::kFailure;
	}

	// Parse values
	if (argParser.isFlagSet(kHoverFlagLong))
		status = setResult(m_context->getHoverManipPlug());
	else if (argParser.isFlagSet(kActiveFlagLong))
		status = setResult(m_context->getActiveManipPlug());

	return status;
}

MStatus FlexiChainTriple_ScaleAdjustmentContextCommand::appendSyntax()
{
	MStatus status;

	MSyntax thisSyntax = syntax();
	status = thisSyntax.addFlag(kHoverFlag, kHoverFlagLong, MSyntax::kString);
	status = thisSyntax.addFlag(kActiveFlag, kActiveFlagLong, MSyntax::kString);

	thisSyntax.enableQuery(true);
	thisSyntax.enableEdit(true);

	return status;
}

// Cleanup
#undef kErrorInvalidFlagCombination

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------