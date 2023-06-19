#include "flexiInstancer_locator.h"
#include "flexiInstancer_locator_upVector_manip.h"

/*	Description
	-----------
	The below setup is designed to provide a way to manually override the up-vector of a FlexiInstancer node
	- The up-vector of a rig is usually stable within a limited range of motion
	- When an exaggerated pose is required, it may be useful to have an option which allows that range of motion to be extended
	- The accompanying MPxSubSceneOverride is designed to work synchronously with this system
	- Visual feedback of the up-vector instability is provided so that the user can make necessary adjustments

	Interface
	---------
	Plugin registration instantiates a single instance of the FlexiInstancer_UpVectorContext (MPxSelectionContext) which will be named "FlexiInstancerUpVectorContext1"
	A custom marking menu is also registered as a callback, such that right clicking a FlexiInstancer node will bring up a menu allowing the user to activate the context
	Once activated, the context retrieves data from attributes on each FlexiInstancer node that is registered to the active context
	Each node provides data to two manipulators, a start and end FlexiInstancer_UpVectorManip (MPxManipulatorNode)
	Both manipulators are encapsulated by a FlexiInstancer_UpVectorManipContainer (MPxManipContainer) because of a selection issue detailed below
	The manipulators can be interacted with by the user to update and animate the attribute values
	The callback mentioned above will first check if the mouse is hovered over a manipulator handle, providing functions specific to that manipulator if this is the case

	Considerations
	--------------
	There are two options for applying a container to a node:
	1. We can apply the container to a specific type of node by associating its name with the node type
	- This allows us to use the "Show Manipulator Tool" on a selected node of that type
	- Maya will create the manipulator for us, adding it into the DAG
	- This method is simple but will always create a fixed number of manipulators and is therefore limited
	2. We can create a custom context which gives us finer control over how manipulators are created and deleted
	- We can use the context to update the number of manipulators that should be intantiated each time the context is activated

	Maya's MPxManipContainer class provides a reasonably simple interface for connecting default base manipulators to plugs
	- Unfortunately this same interface is not compatible with custom MPxManipulatorNodes
	- Updating plug values from a MPxManipulatorNode is fairly trivial once the interaction methods have been implemented
	- However, if we want the manipulator to update when a plug value changes, custom callbacks will need to be used to track when a change occurs

	Issues / Bugs
	-------------
	MPxManipContainers do not seem to be designed to contain multiple base manipulators of the same type
	- If this occurs, the converter will not increment indices for certain properties
	- For instance, the indices given by MFnDirectionManip::directionIndex() do increment however the indices for MFnDirectionManip::startIndex() do not
	- The result is multiple base manipulators with the same indices representing the same properties
	- If callbacks are then installed for properties which have not incremented, the converter will return the same values when queried
	- A solution is to just create a MPxManipContainer instance for each base manipulator
	
	The above setup is not really necessary for MPxManipulatorNodes as manipulator values are generally updated manually
	- If this setup is used, be aware the index returned by MPxManipulatorNode::glActiveName() is only affected by manipulators assigned to the same container
	- Therefore once selected, this function will always return the index of this handle as the container only holds a single manipulator
	- This can cause confusion because visually Maya will only show one active manipulator inside the active context
	- However internally there can be an active manipulator per container assigned to the context

	MPxManipulatorNodes present the following issues:
	1. The following comment in Maya's documentation of MPxManipulatorNode is misleading (it is in relation to the drawUI() and draw() functions)
	- "Selection relies on the default viewport draw pass so the draw() method must still be implemented even if the manipulator is not intended for use in the default viewport"
	- This is not true, selection should be implemented in drawUI()
	2. Adding a MPxManipulatorNode directly to a custom context blocks the manipulator from being selected
	- Adding the MPxManipulatorNode to a MPxManipContainer then adding the container to the custom context resolves this issue
	3. Maya's documentation outlines a way to set up an association between a manipulator value and a plug on a dependency node
	- The final step of this process is to "call set*Value() in one of the do*() virtuals", however doing so seems to crash Maya
	- Instead, we must set the value on the plug directly which essentially defeats the purpose of this setup

	There are certain issues caused by the MUIDrawManager:
	1. Maya does not deal well with drawing of large text items and will crash if they become too large
	2. Maya also does not deal well with continuous changes to the font size (especially when coupled with the above issue)
	- We have text whose font size is proportional to the camera distance but is clamped to a maximum
	- Basically, if the camera distance changes too quickly, Maya's performance will suddenly drop and never recover (fps will be limited to around 80-100)
	- Currently, the issue does not seem to occur when a max font size of 16 is used, however if future performance issues occur this should be looked at first
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiInstancer_UpVectorManip::FlexiInstancer_UpVectorManip() : MPxManipulatorNode(),
	m_locator{ nullptr },
	m_directionIndex{ 0 },
	m_activeName{ 0 },
	m_handleName{ 0 },
	m_isEnabled{ false },
	m_pLineStart{ 0.0f, 0.0f, 0.0f, 1.0f },
	m_pLineEnd{ 0.0f, 10.0f, 0.0f, 1.0f },
	m_isUserInteracting{ false },
	m_isMouseHovered{ false },
	m_isActive{ false }
{
	m_positionIndex = positionIndex;
}

FlexiInstancer_UpVectorManip::~FlexiInstancer_UpVectorManip()
{
	m_locator = nullptr;
}

// Static
unsigned int FlexiInstancer_UpVectorManip::positionIndex = 0;
const MDistance FlexiInstancer_UpVectorManip::m_handleRadius{ 1.5, MDistance::kCentimeters };

const float FlexiInstancer_UpVectorManip::m_tickPoints[][4]
{
	{-4.419f, 0.0f, 0.0f, 1.0f},
	{-6.187f, -1.768f, 0.0f, 1.0f},
	{-2.652f, -5.303f, 0.0f, 1.0f},
	{-4.419f, 0.0f, 0.0f, 1.0f},
	{-0.884f, -3.536f, 0.0f, 1.0f},
	{-2.652f, -5.303f, 0.0f, 1.0f},
	{4.419f, 5.303f, 0.0f, 1.0f},
	{-2.652f, -1.768f, 0.0f, 1.0f},
	{-0.884f, -3.536f, 0.0f, 1.0f},
	{4.419f, 5.303f, 0.0f, 1.0f},
	{6.187f, 3.536f, 0.0f, 1.0f},
	{-0.884f, -3.536f, 0.0f, 1.0f}
};

const float FlexiInstancer_UpVectorManip::m_crossPoints[][4]
{
	{-3.536f, 5.303f, 0.0f, 1.0f},
	{-5.303f, 3.536f, 0.0f, 1.0f},
	{3.536f, -5.303f, 0.0f, 1.0f},
	{-3.536f, 5.303f, 0.0f, 1.0f},
	{5.303f, -3.536f, 0.0f, 1.0f},
	{3.536f, -5.303f, 0.0f, 1.0f},
	{3.536f, 5.303f, 0.0f, 1.0f},
	{5.303f, 3.536f, 0.0f, 1.0f},
	{-3.536f, -5.303f, 0.0f, 1.0f},
	{3.536f, 5.303f, 0.0f, 1.0f},
	{-5.303f, -3.536f, 0.0f, 1.0f},
	{-3.536f, -5.303f, 0.0f, 1.0f}
};

// ------ MPxNode------

void FlexiInstancer_UpVectorManip::postConstructor()
{
	// Enable for hover state tracking
	registerForMouseMove();

	// Assign indices for selectable handles
	MGLuint glPickableItem;
	glFirstHandle(glPickableItem);
	m_handleName = glPickableItem;

	// Add values which will be connected to plugs
	addVectorValue("direction", m_pLineEnd , m_directionIndex);
}

// ------ MPxManipulatorNode ------

MStatus FlexiInstancer_UpVectorManip::initialize()
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
MStatus FlexiInstancer_UpVectorManip::connectToDependNode(const MObject& node)
{
	MStatus status;

	MDagPath::getAPathTo(node, m_locatorPath);
	MFnDependencyNode fnDep{ node };
	m_locator = (FlexiInstancer*)fnDep.userNode();
	
	// Direction Mapping
	MPlug directionPlug;
	status = getDirectionPlug(directionPlug);
	if (status)
	{
		int plugIndex;
		connectPlugToValue(directionPlug, m_directionIndex, plugIndex);
	}

	// Update initial values
	updateManipPoints();
	updateManipState();
	
	// Initialize converter
	finishAddingManips();
	MPxManipulatorNode::connectToDependNode(node);

	return MStatus::kSuccess;
}

void FlexiInstancer_UpVectorManip::preDrawUI(const M3dView& view)
{
	// The start and end points resulting from user interaction are cached as member variables but may need to be updated for drawing
	// If these values have been animated or the shape of the curve has changed as a result of animation, the data will be stale
	updateManipPoints();
	updateManipState();
}

/*	Description
	-----------
	Function is responsible for implementing the draw and selection interfaces

	Considerations
	--------------
	The draw() method does not need to be implemented despite what the documentation says
	2D drawables are being avoided as they are not allowing selection and must use screen space pixel scaling
	We want our items to be proportional to the camera distance which can be achieved with 2D drawables but is not really the intention
	The font size of text needs to be limited, see notes on issues/bugs above    */
void FlexiInstancer_UpVectorManip::drawUI(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext) const
{
	const short manipMainColor = mainColor();
	const short manipLineColor = lineColor();
	MPoint pCameraWorld = getCameraPosition(frameContext);

	// State Icon
	MPointArray iconPoints;
	MColor iconColor;
	if (m_isEnabled)
	{
		iconPoints = MPointArray{ m_tickPoints, 12 };
		iconColor = MColor{ 0.05f, 1.0f, 0.05f };
	}
	else
	{
		iconPoints = MPointArray{ m_crossPoints, 12 };
		iconColor = MColor{ 1.0f, 0.05f, 0.05f };
	}

	// By default, points are positioned at the world origin and are aiming in +z
	// Transform the points so they are scaled down, repositioned and are aiming towards the camera
	MPoint pIconOffset = m_pLineEnd;
	MVector vAim = pIconOffset - pCameraWorld;
	vAim.normalize();
	MVector vUp = getCameraUpDirection(frameContext);
	MVector vRight = vAim ^ vUp;
	vRight.normalize();
	vUp = vRight ^ vAim;
	MMatrix iconTransform = MRS::matrixFromVectors(vRight, vUp, vAim, pIconOffset);
	for (unsigned int i = 0; i < iconPoints.length(); ++i)
	{
		iconPoints[i] *= 0.15;
		iconPoints[i] *= iconTransform;
	}

	drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
	drawManager.setColor(iconColor);
	drawManager.setPaintStyle(MHWRender::MUIDrawManager::kFlat);
	drawManager.mesh(MHWRender::MUIDrawManager::Primitive::kTriangles, iconPoints);
	drawManager.endDrawable();
	
	// Text
	MVector vCameraToDrawable = m_pLineStart - pCameraWorld;
	double cameraToDrawableLength = vCameraToDrawable.length();
	drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
	MString indexText = std::to_string(m_positionIndex).c_str();
	MPoint pText = m_pLineEnd + 2.0 * vUp;
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
	
	// Line
	drawManager.beginDrawable(MHWRender::MUIDrawManager::kNonSelectable);
	drawManager.setColorIndex(manipLineColor);
	drawManager.line(m_pLineStart, m_pLineEnd);
	drawManager.endDrawable();

	// Circle End
	vCameraToDrawable = m_pLineEnd - pCameraWorld;
	vCameraToDrawable.normalize();
	// We draw two circles so that the entire circle is selectable, has a transparent fill and has a solid border
	drawManager.beginDrawable(MHWRender::MUIDrawManager::kSelectable, m_handleName);
	drawManager.setColor(MColor(0.0));
	drawManager.circle(m_pLineEnd, vCameraToDrawable, m_handleRadius.asUnits(MDistance::uiUnit()), true);
	// Sets color used when the item is neither highlighted or selected
	setHandleColor(drawManager, m_handleName, manipMainColor);
	drawManager.setDepthPriority(MRenderItem::sSelectionDepthPriority);
	drawManager.circle(m_pLineEnd, vCameraToDrawable, m_handleRadius.asUnits(MDistance::uiUnit()), false);
	drawManager.endDrawable();
}

MStatus FlexiInstancer_UpVectorManip::doPress(M3dView& view)
{
	if (glActiveName(m_activeName))
	{
		if (m_activeName == m_handleName)
		{
			// Because of the way MPxSelectionContext handles selection, we are manually tracking which manipulator is visually selected (see notes on issues at top)
			// Essentially, a context with multiple containers can internally have multiple active manipulators (one per container)
			// However Maya will only display the last selected manipulator as being visually active
			MString activePlugCmd = "FlexiInstancerUpVectorContext -e -activeManipPlug " + getCompoundPlugPartialName();
			activePlugCmd += " FlexiInstancerUpVectorContext1";
			MGlobal::executeCommand(activePlugCmd);

			MGlobal::executeCommand("undoInfo -openChunk");
			m_isUserInteracting = true;

			MPoint pCameraWorld = getCameraPosition(view);

			// We want to calculate the normal of the intersection plane here so that it is not continually updating during a drag
			// If it were continually updating during a drag, then we would not be dragging along a static plane (ie. the plane would be rotating as we were dragging)
			// The selected item is always facing the camera, so the normal of the intersection plane will always be the vector from the camera to the item
			// However, we want to calculate the normal in the same direction as the view (ie. from camera to item) as we will be projecting onto it
			m_vInterPlaneNormalHandle = m_pLineEnd - pCameraWorld;
			m_vInterPlaneNormalHandle.normalize();

			// Update the mouse start position
			computeCurrentMouseIntersection(m_pLineEnd, m_vInterPlaneNormalHandle, m_pMouseStartHandle);

			// Cache the vector from the end point to the actual point on the manipulator that the user has selected
			// We will maintain this offset during the drag as it prevents the center of the manip from jumping to the mouse position
			m_vMousePressOffsetHandle = m_pMouseStartHandle - m_pLineEnd;
		}
	}

	return MStatus::kSuccess;
}

/*	Description
	-----------
	Function finds the end position of the manipulator
	- The mouse start and end positions are raycast onto a static intersection plane and are used to calculate a delta
	- The intersection plane is centered at the previous end position of the manipulator and is normal to the camera    */
MStatus FlexiInstancer_UpVectorManip::doDrag(M3dView& view)
{
	MStatus status;

	if (glActiveName(m_activeName))
	{
		if (m_activeName == m_handleName)
		{
			// Update the mouse end position
			computeCurrentMouseIntersection(m_pLineEnd, m_vInterPlaneNormalHandle, m_pMouseEndHandle);

			// Calculate the delta between mouse start and end
			MVector vMouseDelta = m_pMouseEndHandle - m_pMouseStartHandle;
			// Apply the delta to the position of the handle but limit how far away it is from the start point
			m_pLineEnd += vMouseDelta;
			MVector vOffset = m_pLineEnd - m_pLineStart;
			vOffset.normalize();
			m_pLineEnd = m_pLineStart + vOffset * 10;
			// Localize the offset
			MVector vOffsetLocalized = vOffset * m_locatorPath.inclusiveMatrixInverse();
			vOffsetLocalized.normalize();
			// Make the mouse start position the new end position and ensure the initial offset is maintained
			m_pMouseStartHandle = m_pLineEnd + m_vMousePressOffsetHandle;

			status = updatePlugDirection(vOffsetLocalized);
		}
	}

	return status;
}

MStatus FlexiInstancer_UpVectorManip::doRelease(M3dView& view)
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
MStatus FlexiInstancer_UpVectorManip::doMove(M3dView& view, bool& refresh)
{
	MPoint pCameraWorld = getCameraPosition(view);

	MVector vInterPlaneNormalHandle = m_pLineEnd - pCameraWorld;
	vInterPlaneNormalHandle.normalize();

	// Calculate the mouse intersection on the normal plane
	MPoint pInterMouseHandle;
	computeCurrentMouseIntersection(m_pLineEnd, vInterPlaneNormalHandle, pInterMouseHandle);

	// Find the offset from the center of the plane
	MVector vMouseOffsetHandle = m_pLineEnd - pInterMouseHandle;

	// If the offset is smaller than the handle's radius then the mouse is within the hover region
	// The MUIDrawManager adds a hover buffer region around items which appears to be proportional to the items' distance from the camera
	// The below calculation attempts to mimic this hover buffer region with fairly high accuracy (not actually sure how it works internally)
	MPoint pCamera = getCameraPosition(view);
	MVector vCameraToHandle = m_pLineEnd - pCamera;
	// The max multiplier of 4.0 occurs at 10 meters
	MDistance refDist{ 1000, MDistance::kCentimeters };
	double hoverRadiusMult = vCameraToHandle.length() / refDist.asUnits(MDistance::uiUnit()) * 4.0;
	// Clamp the multiplier to the max (ie. past 10 meters the multiplier is always 4.0)
	hoverRadiusMult = std::min(hoverRadiusMult, 4.0);
	// Remap the multiplier so the low is always 1.0
	// Remap = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
	hoverRadiusMult = 1.0 + hoverRadiusMult * 3.0 / 4.0;

	if (vMouseOffsetHandle.length() <= m_handleRadius.asUnits(MDistance::uiUnit()) * hoverRadiusMult)
		m_isMouseHovered = true;
	else
		m_isMouseHovered = false;

	return MStatus::kSuccess;
}

// ------ Helpers ------

/*	Description
	-----------
	Function retrieves the start or end compound plug associated with this manipulator    */
MStatus FlexiInstancer_UpVectorManip::getCompoundPlug(MPlug& compoundPlug)
{
	MStatus status;
	
	if (m_positionIndex == 0)
		compoundPlug = MPlug{ m_locatorPath.node(), FlexiInstancer::normalUpVectorOverrideCompoundAttr };
	else if (m_positionIndex == 1)
		compoundPlug = MPlug{ m_locatorPath.node(), FlexiInstancer::counterTwistUpVectorOverrideCompoundAttr };

	return status;
}

/*	Description
	-----------
	Function retrieves the direction plug associated with this manipulator    */
MStatus FlexiInstancer_UpVectorManip::getDirectionPlug(MPlug& directionPlug)
{
	MStatus status;

	MPlug compoundPlug;
	status = getCompoundPlug(compoundPlug);
	if (status)
	{
		if (m_positionIndex == 0)
			directionPlug = compoundPlug.child(FlexiInstancer::normalUpVectorOverrideAttr, &status);
		else if (m_positionIndex == 1)
			directionPlug = compoundPlug.child(FlexiInstancer::counterTwistUpVectorOverrideAttr, &status);
	}

	return status;
}

/*	Description
	-----------
	Function retrieves the state plug associated with this manipulator    */
MStatus FlexiInstancer_UpVectorManip::getStatePlug(MPlug& statePlug)
{
	MStatus status;

	MPlug compoundPlug;
	status = getCompoundPlug(compoundPlug);
	if (status)
	{
		if (m_positionIndex == 0)
			statePlug = compoundPlug.child(FlexiInstancer::normalUpVectorOverrideStateAttr, &status);
		else if (m_positionIndex == 1)
			statePlug = compoundPlug.child(FlexiInstancer::counterTwistUpVectorOverrideStateAttr, &status);
	}

	return status;
}

/*	Description
	-----------
	Function returns the partial node.plug name of the compound plug associated with this manipulator (ensuring both node and plug are partial)    */
MString FlexiInstancer_UpVectorManip::getCompoundPlugPartialName()
{
	MString locatorName = m_locatorPath.partialPathName();
	MPlug compoundPlug;
	MStatus status = getCompoundPlug(compoundPlug);
	return status ? locatorName + "." + compoundPlug.partialName() : "";
}

bool FlexiInstancer_UpVectorManip::isEnabled()
{
	MStatus status;

	bool enabled = true;
	MPlug statePlug;
	status = getStatePlug(statePlug);
	if (status)
		enabled = statePlug.asBool();

	return enabled;
}

bool FlexiInstancer_UpVectorManip::isUserInteracting()
{
	return m_isUserInteracting;
}

bool FlexiInstancer_UpVectorManip::isMouseHovered()
{
	return m_isMouseHovered;
}

bool FlexiInstancer_UpVectorManip::isActive()
{
	return m_isActive;
}

// Legacy implementation
MPoint FlexiInstancer_UpVectorManip::getCameraPosition(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MPoint pCameraWorld = MFnTransform(cameraPath).rotatePivot(MSpace::kWorld);
	return pCameraWorld;
}

// VP2 implementation
MPoint FlexiInstancer_UpVectorManip::getCameraPosition(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray viewPosition = frameContext.getTuple(MHWRender::MFrameContext::kViewPosition);
	MPoint pCameraWorld{ viewPosition[0], viewPosition[1], viewPosition[2] };
	return pCameraWorld;
}

// Legacy implementation
MVector FlexiInstancer_UpVectorManip::getCameraAimDirection(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MFnCamera cameraFn{ cameraPath };
	MVector vAimDirection = cameraFn.viewDirection(MSpace::kWorld);
	return vAimDirection;
}

// VP2 implementation
MVector FlexiInstancer_UpVectorManip::getCameraAimDirection(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray aimDirection = frameContext.getTuple(MHWRender::MFrameContext::kViewDirection);
	MPoint vAimDirection{ aimDirection[0], aimDirection[1], aimDirection[2] };
	return vAimDirection;
}

// Legacy implementation
MVector FlexiInstancer_UpVectorManip::getCameraUpDirection(M3dView& view) const
{
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	cameraPath.pop();
	MFnCamera cameraFn{ cameraPath };
	MVector vUpDirection = cameraFn.upDirection(MSpace::kWorld);
	return vUpDirection;
}

// VP2 implementation
MVector FlexiInstancer_UpVectorManip::getCameraUpDirection(const MHWRender::MFrameContext& frameContext) const
{
	MDoubleArray upDirection = frameContext.getTuple(MHWRender::MFrameContext::kViewUp);
	MPoint vUpDirection{ upDirection[0], upDirection[1], upDirection[2] };
	return vUpDirection;
}

/*	Description
	-----------
	Calculates the intersection of the given mouse position and direction raycast onto the plane determined by the given plane data
	Returns true if the raycast produces a successful intersection    */
bool FlexiInstancer_UpVectorManip::computeMouseIntersection(const MPoint& pMouse, const MVector& vMouseDirection, const MPoint& pPointOnInterPlane, 
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
bool FlexiInstancer_UpVectorManip::computeCurrentMouseIntersection(const MPoint& pPointOnInterPlane, const MVector& vInterPlaneNormal, MPoint& pIntersection)
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

void FlexiInstancer_UpVectorManip::updateManipState()
{
	m_isEnabled = isEnabled();
}

void FlexiInstancer_UpVectorManip::updateManipPoints()
{
	const FlexiInstancer::FlexiInstancer_Data& curveData = m_locator->getCurveData();
	if (m_positionIndex == 0)
		m_pLineStart = curveData.vLowerBoundPoint * m_locatorPath.inclusiveMatrix();
	else
		m_pLineStart = curveData.vUpperBoundPoint * m_locatorPath.inclusiveMatrix();

	// Retrieve local space up-vector
	MVector vLocalUp;
	getVectorValue(m_directionIndex, false, vLocalUp);

	// Apply locator's rotation to the combined vector
	MVector vRotatedUp = vLocalUp * m_locatorPath.inclusiveMatrix();
	// Normalize in case locator has scaling applied
	vRotatedUp.normalize();
	vRotatedUp *= 10.0;

	m_pLineEnd = m_pLineStart + vRotatedUp;
}

/*	Description
	-----------
	This method exists to resolve crashing issues caused when using setVectorValue methods
	It updates the up-vector child plug for the compound plug associated with this manipulator    */
MStatus FlexiInstancer_UpVectorManip::updatePlugDirection(MVector& vDirection)
{
	MStatus status;

	MPlug directionPlug;
	status = getDirectionPlug(directionPlug);
	if (status)
	{
		MObject dataObj;
		MFnNumericData dataFn;
		dataObj = dataFn.create(MFnNumericData::k3Double);
		dataFn.setData(vDirection.x, vDirection.y, vDirection.z);
			
		// Ownership will be transferred to the command object
		MDGModifier* dgMod = new MDGModifier();
		dgMod->newPlugValue(directionPlug, dataObj);
		status = MRS::executeUndoModifierCmd(dgMod);
	}

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiInstancer_UpVectorManipContainer::FlexiInstancer_UpVectorManipContainer() : MPxManipContainer(),
	m_manip{ nullptr },
	m_locator{ nullptr }
{}

FlexiInstancer_UpVectorManipContainer::~FlexiInstancer_UpVectorManipContainer()
{
	m_manip = nullptr;
	m_locator = nullptr;
}

// ------ MPxManipContainer ------

MStatus FlexiInstancer_UpVectorManipContainer::initialize()
{
	return MPxManipContainer::initialize();
}

MStatus FlexiInstancer_UpVectorManipContainer::createChildren()
{
	MStatus status;
	
	MPxManipulatorNode *proxyManip = 0;
	MString manipTypeName(FlexiInstancer_UpVectorManip::kTypeName);
	MString manipName("FlexiInstancerManipInst");
	status = addMPxManipulatorNode(manipTypeName, manipName, proxyManip);

	if (status)
		m_manip = (FlexiInstancer_UpVectorManip*)proxyManip;

	return status;
}

MStatus FlexiInstancer_UpVectorManipContainer::connectToDependNode(const MObject& node)
{
	MStatus status;

	MDagPath::getAPathTo(node, m_locatorPath);
	MFnDependencyNode fnDep{ node };
	m_locator = (FlexiInstancer*)fnDep.userNode();
	m_manip->connectToDependNode(node);

	// Initialize converter
	finishAddingManips();
	MPxManipContainer::connectToDependNode(node);

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiInstancer_UpVectorContext::FlexiInstancer_UpVectorContext() : MPxSelectionContext(),
	m_numActiveNodes{ 0 },
	m_selectionChangedCallbackId{ 0 }
{
	MString titleStr("FlexiInstancer Up-Vector Tool");
	setTitleString(titleStr);
}

FlexiInstancer_UpVectorContext::~FlexiInstancer_UpVectorContext() {}

// ------ MPxSelectionContext ------

/*	Description
	-----------
	This method is called when the context is activated
	It instantiates manipulators for the currently hilited FlexiInstancer nodes
	It adds a callback which is invoked when the selection changes so that manipulators can be reinstantiated
	It adds a callback for each hilited node, responsible for tracking attribute changes and updating the associated manipulator values    */
void FlexiInstancer_UpVectorContext::toolOnSetup(MEvent& event)
{
	MStatus status;

	// Enter component mode but disable selection masks for components (we are just using component mode as a visual indicator)
	MGlobal::executeCommand("selectMode -component");
	MGlobal::executeCommand("selectType -allComponents false");

	MString helpStr("Drag manipulators to define the up-vector to be used at each end. Right click for more options.");
	setHelpString(helpStr);

	// Add a callback for selection changes
	m_selectionChangedCallbackId = MEventMessage::addEventCallback("SelectionChanged", updateManipulatorsCallback, this, &status);
	if (!status)
		std::cerr << "FlexiInstancer_UpVectorContext: MEventMessage::addEventCallback() failed" << std::endl;

	// Update manipulators
	updateManipulatorsCallback(this);
}

/*	Description
	-----------
	This method is called if an instance of this context is active when the setToolTo command is executed
	Even if the context being set is already active, this function will still be invoked, effectively refreshing the context
	It undoes the work of the setup function, removing all MPxManipContainers and all callbacks which are owned by the current instance of this context     */
void FlexiInstancer_UpVectorContext::toolOffCleanup()
{
	MStatus status;

	MGlobal::executeCommand("selectMode -object");
	MGlobal::executeCommand("selectType -allComponents true");
	// After the context is deactivated, text from previous drawFeedback() calls does not update until the user moves the camera, force this update
	MGlobal::executeCommandOnIdle("refresh");

	// Remove callbacks and reset variables
	status = MNodeMessage::removeCallback(m_selectionChangedCallbackId);
	if (!status)
		cerr << "FlexiInstancer_UpVectorContext: MNodeMessage::removeCallback() failed" << std::endl;
	m_selectionChangedCallbackId = 0;

	for (MCallbackId& id : m_nodePlugChangedCallbackIds)
	{
		status = MNodeMessage::removeCallback(id);
		if (!status)
			cerr << "FlexiInstancer_UpVectorContext: MNodeMessage::removeCallback() failed" << std::endl;
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
MStatus FlexiInstancer_UpVectorContext::doEnterRegion(MEvent& event)
{
	MString helpStr("Drag manipulators to define the up-vector to be used at each end. Right click for more options.");
	return setHelpString(helpStr);
}

/*	Description
	-----------
	Draws contextual information about the current state of the context     */
MStatus FlexiInstancer_UpVectorContext::drawFeedback(MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext)
{
	int viewportOriginX, viewportOriginY, viewportWidth, viewportHeight;
	frameContext.getViewportDimensions(viewportOriginX, viewportOriginY, viewportWidth, viewportHeight);
	MPoint pText{ (double)viewportWidth / 2, (double)viewportHeight * 0.97, 0 };

	drawManager.setFontSize(13);
	MString activeItems = std::to_string(m_numActiveNodes).c_str();
	MString text;
	if (m_numActiveNodes == 1)
		text = ("FlexiInstancer  :  Up-Vector Tool  :  " + activeItems) + " Active Node";
	else
		text = ("FlexiInstancer  :  Up-Vector Tool  :  " + activeItems) + " Active Nodes";
	drawManager.text2d(pText, text, MHWRender::MUIDrawManager::TextAlignment::kCenter, nullptr, nullptr, false);
	drawManager.endDrawable();

	return MStatus::kSuccess;
}

// ------ Helpers ------

/*	Description
	-----------
	This function is invoked by our callback which responds to RMB events on FlexiInstancer nodes
	If the mouse is currently hovered over a manipulator which belongs to this context, it will return the partial name of the plug to which the manipulator is associated
	If the mouse is not hovered over a manipulator, an empty string will be returned    */
MString FlexiInstancer_UpVectorContext::getHoverManipPlug()
{
	for (FlexiInstancer_UpVectorManipContainer*& manipContainer : m_manipContainers)
	{
		if (manipContainer->m_manip->isMouseHovered())
			return manipContainer->m_manip->getCompoundPlugPartialName();
	}

	return MString();
}

/*	Description
	-----------
	This function is invoked by the associated MPxSubSceneOverride registered to FlexiInstancer nodes
	If a manipulator which belongs to this context is currently active, it will return the partial name of the plug to which the manipulator is associated
	If no manipulator is active, an empty string will be returned    */
MString FlexiInstancer_UpVectorContext::getActiveManipPlug()
{
	for (FlexiInstancer_UpVectorManipContainer*& manipContainer : m_manipContainers)
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
void FlexiInstancer_UpVectorContext::setActiveManipPlug(MString& activeManipPlugName)
{
	MStatus status;

	MSelectionList selList;
	selList.add(activeManipPlugName);
	MPlug activeManipPlug;
	status = selList.getPlug(0, activeManipPlug);

	if (status)
	{
		for (FlexiInstancer_UpVectorManipContainer*& manipContainer : m_manipContainers)
		{
			MPlug manipPlug;
			status = manipContainer->m_manip->getCompoundPlug(manipPlug);

			if (status && manipPlug == activeManipPlug)
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
void FlexiInstancer_UpVectorContext::updateManipulatorsCallback(void* data)
{
	MStatus status;

	FlexiInstancer_UpVectorContext* context = (FlexiInstancer_UpVectorContext*)data;
	context->m_numActiveNodes = 0;

	// Remove callbacks for when selection has changed
	for (MCallbackId& id : context->m_nodePlugChangedCallbackIds)
	{
		status = MNodeMessage::removeCallback(id);
		if (!status)
			cerr << "FlexiInstancer_UpVectorContext: MNodeMessage::removeCallback() failed" << std::endl;
	}
	context->m_nodePlugChangedCallbackIds.clear();

	// Delete manipulators
	context->m_manipContainers.clear();
	context->deleteManipulators();

	// When the tool is activated, component selection mode is enabled which removes nodes from the active selection list and adds them to the hilite list
	// Create manipulators for each FlexiInstancer object that is currently in hilite display mode and store them with the context so we can delete them later
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

		if (!seen && fnDep.typeId() == FlexiInstancer::kTypeId)
		{
			seenNodes.append(nodeObj);
			++context->m_numActiveNodes;

			// We could either create a container per node with two manipulators each or a container per manipulator
			// MPxManipContainer::createChildren() will be called directly after each new manipulator container is instantiated below
			// The static member "positionIndex" holds transient data as a way of passing it to the constructor so it can be set as a member
			for (unsigned int i = 0; i < 2; ++i)
			{
				FlexiInstancer_UpVectorManip::positionIndex = i;
				MString manipName(FlexiInstancer_UpVectorManipContainer::kTypeName);
				MObject manipObject = MObject::kNullObj;
				FlexiInstancer_UpVectorManipContainer* manipContainer = (FlexiInstancer_UpVectorManipContainer*)FlexiInstancer_UpVectorManipContainer::newManipulator(manipName, manipObject);

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
				std::cerr << "FlexiInstancer_UpVectorContext: MNodeMessage::addAttributeChangedCallback() failed" << std::endl;
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
void FlexiInstancer_UpVectorContext::nodePlugChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* data)
{
	// Note, even though the drag interaction is completed in a single undo chunk, undoing the operation will invoke this callback for each command in the chunk
	// Wait for the below operation to complete and for an output to evaluate before updating manipulators
	if (MGlobal::isUndoing() || MGlobal::isRedoing())
		return;
	
	if (msg & MNodeMessage::AttributeMessage::kAttributeEval)
	{
		if (plug == FlexiInstancer::outputLocalParticleArrayAttr || plug == FlexiInstancer::outputWorldParticleArrayAttr)
		{
			// This command will have the same effect as the below solution, however it resets the current handle selection and is less efficient
			//MGlobal::executeCommandOnIdle("setToolTo `currentCtx`");

			FlexiInstancer_UpVectorContext* context = (FlexiInstancer_UpVectorContext*)data;
			for (FlexiInstancer_UpVectorManipContainer*& manipContainer : context->m_manipContainers)
			{
				if (!manipContainer->m_manip->isUserInteracting())
				{
					manipContainer->m_manip->updateManipPoints();
					manipContainer->m_manip->updateManipState();
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The FlexiInstancerUpVectorContext command is used to generate a custom context which can then be used to set manual overrides for the up-vectors
	There are two up-vectors for which this context provides the ability to set an override
	- The main up-vector which is used to calculate the principal normal of the curve
	- The counter-twist up-vector which is used to generate a target normal vector for the counter-twist calculation

	MEL Command
	-----------
	FlexiInstancerUpVectorContext [-hoverManipPlug bool] [-activeManipPlug bool] [context]

	Flags
	-----
	-hoverManipPlug(-hmp)
		This flag is used to query the currently hovered manipulator handle within the active context, specified using the context argument
		- If the user's mouse is hovered over a manipulator handle, the name of the compound plug associated with that manipulator will be returned
		- If no manipulator handle is currently hovered, an empty string will be returned

	-activeManipPlug(-amp)
		This flag is used to query the currently active manipulator handle within the active context, specified using the context argument
		The active handle is being considered as the one which is visually selected (internally Maya allows an active handle per MPxManipContainer)
		- If a manipulator handle within the given context is active, the name of the compound plug associated with that manipulator will be returned
		- If no manipulator handle is currently active, an empty string will be returned

	Args
	----
	context
		This is the name given to the FlexiInstancerUpVectorContext instance upon creation
		When querying the active context, this name must be passed to the command
*/

FlexiInstancer_UpVectorContextCommand::FlexiInstancer_UpVectorContextCommand() : MPxContextCommand(),
	m_context{ nullptr }
{}

FlexiInstancer_UpVectorContextCommand::~FlexiInstancer_UpVectorContextCommand()
{
	m_context = nullptr;
}

// ------ MPxContextCommand ------

#define kErrorInvalidFlagCombination \
	"Only one of -hoverManipPlug/-activeManipPlug flags may be specified."

const char* FlexiInstancer_UpVectorContextCommand::kHoverFlag = "-hmp";
const char* FlexiInstancer_UpVectorContextCommand::kHoverFlagLong = "-hoverManipPlug";
const char* FlexiInstancer_UpVectorContextCommand::kActiveFlag = "-amp";
const char* FlexiInstancer_UpVectorContextCommand::kActiveFlagLong = "-activeManipPlug";

MPxContext* FlexiInstancer_UpVectorContextCommand::makeObj()
{
	m_context = new FlexiInstancer_UpVectorContext;
	return m_context;
}

MStatus FlexiInstancer_UpVectorContextCommand::doEditFlags()
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

MStatus FlexiInstancer_UpVectorContextCommand::doQueryFlags()
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

MStatus FlexiInstancer_UpVectorContextCommand::appendSyntax()
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