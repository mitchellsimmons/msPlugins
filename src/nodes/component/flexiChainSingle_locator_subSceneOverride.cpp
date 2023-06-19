#include "flexiChainSingle_locator_subSceneOverride.h"
#include "utils/macros.h"

/*	Considerations
	--------------
	One major difference between MPxSubSceneOverride and other overrides is the following quoted from Maya's documentation:
	- "The implementation is free at any time to query any information it needs directly from Maya"
	- Therefore there is no equivalent to MPxGeomtryOverride::updateDG() or MPxDrawOverride::prepareForDraw()

	Another major difference is draw cycles appear to be called much more frequently when using MPxSubSceneOverride (eg. when tumbling the viewport)
	- Our implementation should account for this so that buffers are only updated if the node has reevaluated since the last draw

	Maya's documentation on MRenderItems briefly mentions they are considered transient and should not be held onto
	However the documentation for MSubSceneContainer repeatedly says it is fine to keep a pointer, so for this reason we will store the pointers
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiChainSingle_SubSceneOverride::FlexiChainSingle_SubSceneOverride(const MObject& obj) :
	MHWRender::MPxSubSceneOverride{ obj },
	m_locatorObj{ obj },
	m_displayStatus{ MHWRender::DisplayStatus::kNoStatus },
	m_surfaceVertexCount{ -1 },
	m_curveVertexCount{ -1 },
	m_normalsVertexCount{ -1 },
	m_surfaceRenderItem{ nullptr },
	m_borderActiveRenderItem{ nullptr },
	m_borderDormantRenderItem{ nullptr },
	m_curveActiveRenderItem{ nullptr },
	m_curveDormantRenderItem{ nullptr },
	m_normalsActiveRenderItem{ nullptr },
	m_normalsDormantRenderItem{ nullptr },
	m_hullRenderItem{ nullptr },
	m_boundingBoxRenderItem{ nullptr },
	m_wireframeShader{ nullptr },
	m_surfaceShader{ nullptr },
	m_borderDormantShader{ nullptr },
	m_curveDormantShader{ nullptr },
	m_normalsDormantShader{ nullptr },
	m_hullShader{ nullptr },
	m_samplerState{ nullptr },
	m_rampTexture{ nullptr },
	m_diffuseTexture{ nullptr },
	m_surfacePositionBuffer{ nullptr },
	m_surfaceNormalBuffer{ nullptr },
	m_surfaceTextureBuffer{ nullptr },
	m_curvePositionBuffer{ nullptr },
	m_normalsPositionBuffer{ nullptr },
	m_hullPositionBuffer{ nullptr },
	m_boundingBoxPositionBuffer{ nullptr },
	m_surfaceIndexBuffer{ nullptr },
	m_borderIndexBuffer{ nullptr },
	m_curveIndexBuffer{ nullptr },
	m_normalsIndexBuffer{ nullptr },
	m_hullIndexBuffer{ nullptr },
	m_boundingBoxIndexBuffer{ nullptr }
{
	MStatus status;
	MFnDependencyNode fnNode{ obj, &status };
	m_locator = status ? dynamic_cast<FlexiChainSingle*>(fnNode.userNode()) : nullptr;
	
	if (status)
	{
		m_overrideLevelOfDetailPlug = fnNode.findPlug("overrideLevelOfDetail", false);
		m_castsShadowsPlug = fnNode.findPlug("castsShadows", false);
		m_receiveShadowsPlug = fnNode.findPlug("receiveShadows", false);
		m_boundingBoxEnabled = (bool)m_overrideLevelOfDetailPlug.asShort();
		m_castsShadows = m_castsShadowsPlug.asBool();
		m_receiveShadows = m_receiveShadowsPlug.asBool();
	}
}

FlexiChainSingle_SubSceneOverride::~FlexiChainSingle_SubSceneOverride()
{ 
	m_locator = nullptr;

	m_surfaceRenderItem = nullptr;
	m_borderActiveRenderItem = nullptr;
	m_borderDormantRenderItem = nullptr;
	m_curveActiveRenderItem = nullptr;
	m_curveDormantRenderItem = nullptr;
	m_normalsActiveRenderItem = nullptr;
	m_normalsDormantRenderItem = nullptr;
	m_hullRenderItem = nullptr;
	m_boundingBoxRenderItem = nullptr;

	deleteGeometryBuffers();

	MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	if (!renderer)
	{
		std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to release shaders" << std::endl;
		return;
	}

	const MHWRender::MShaderManager* shaderManager = renderer->getShaderManager();
	if (!shaderManager)
	{
		std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to release shaders" << std::endl;
		return;
	}

	MHWRender::MStateManager::releaseSamplerState(m_samplerState);
	m_samplerState = nullptr;

	if (m_wireframeShader)
		shaderManager->releaseShader(m_wireframeShader);
	if (m_surfaceShader)
		shaderManager->releaseShader(m_surfaceShader);
	if (m_borderDormantShader)
		shaderManager->releaseShader(m_borderDormantShader);
	if (m_curveDormantShader)
		shaderManager->releaseShader(m_curveDormantShader);
	if (m_normalsDormantShader)
		shaderManager->releaseShader(m_normalsDormantShader);
	if (m_hullShader)
		shaderManager->releaseShader(m_hullShader);

	m_surfaceShader = nullptr;
	m_wireframeShader = nullptr;
	m_borderDormantShader = nullptr;
	m_curveDormantShader = nullptr;
	m_normalsDormantShader = nullptr;
	m_hullShader = nullptr;
}

// ------ Constants ------
const char* FlexiChainSingle_SubSceneOverride::m_surfaceRenderItemName = "FlexiChainSingleSurface";
const char* FlexiChainSingle_SubSceneOverride::m_borderActiveRenderItemName = "FlexiChainSingleActiveBorder";
const char* FlexiChainSingle_SubSceneOverride::m_borderDormantRenderItemName = "FlexiChainSingleDormantBorder";
const char* FlexiChainSingle_SubSceneOverride::m_curveActiveRenderItemName = "FlexiChainSingleActiveCurve";
const char* FlexiChainSingle_SubSceneOverride::m_curveDormantRenderItemName = "FlexiChainSingleDormantCurve";
const char* FlexiChainSingle_SubSceneOverride::m_normalsActiveRenderItemName = "FlexiChainSingleActiveNormals";
const char* FlexiChainSingle_SubSceneOverride::m_normalsDormantRenderItemName = "FlexiChainSingleDormantNormals";
const char* FlexiChainSingle_SubSceneOverride::m_hullRenderItemName = "FlexiChainSingleHull";
const char* FlexiChainSingle_SubSceneOverride::m_boundingBoxRenderItemName = "FlexiChainSingleBoundingBox";

const unsigned int FlexiChainSingle_SubSceneOverride::m_texturedPixelCount = 2048;
const float FlexiChainSingle_SubSceneOverride::m_ribbonWidthScale = 2.3f;
const float FlexiChainSingle_SubSceneOverride::m_normalsLengthScale = 4.0;
const MColor FlexiChainSingle_SubSceneOverride::m_surfaceColor{ 0.5f, 0.5f, 0.5f };
const MColor FlexiChainSingle_SubSceneOverride::m_borderDormantColor{ 0.0f, 0.0f, 0.0f };
const MColor FlexiChainSingle_SubSceneOverride::m_curveDormantColor = MRS::isColorManaged() ?
	MRS::convertDisplayToLinear(MColor{ MColor::kRGB, (unsigned char)45, (unsigned char)49, (unsigned char)66, (unsigned char)255 }) :
	MColor{ MColor::kRGB, (unsigned char)45, (unsigned char)49, (unsigned char)66, (unsigned char)255 };
const MColor FlexiChainSingle_SubSceneOverride::m_normalsDormantColor = MRS::isColorManaged() ?
	MRS::convertDisplayToLinear(MColor{ MColor::kRGB, (unsigned char)159, (unsigned char)70, (unsigned char)68, (unsigned char)255 }) :
	MColor{ MColor::kRGB, (unsigned char)159, (unsigned char)70, (unsigned char)68, (unsigned char)255 };

// ------ MHWRender::MPxDrawOverride ------

MHWRender::DrawAPI FlexiChainSingle_SubSceneOverride::supportedDrawAPIs() const
{
	return MHWRender::kAllDevices;
}

/*	Description
	-----------
	If this function returns false then update() will not be called for the draw-preparation phase
	This would only be the case if the vertex buffers and render items did not need to be updated    */
bool FlexiChainSingle_SubSceneOverride::requiresUpdate(const MSubSceneContainer& container, const MFrameContext& frameContext) const
{
	return true;
}

/*	Description
	-----------
	This function creates/updates the MRenderItems
	It creates/updates the associated MShaderInstance for each render item
	It updates the vertex and index buffers for each visible render item    */
void FlexiChainSingle_SubSceneOverride::update(MSubSceneContainer& container, const MFrameContext& frameContext)
{
	MDagPath path;
	MDagPath::getAPathTo(m_locatorObj, path);
	if (!path.isValid())
	{
		std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to find a valid DAG path" << std::endl;
		return;
	}

	// Check if any locator input is dirty and evaluate if needed
	// Otherwise check if drawing has already occurred since the last evaluation and if so signal to exit early
	bool updateBuffers = true;
	MDataBlock dataBlock = m_locator->getDataBlock();
	if (!dataBlock.outputValue(FlexiChainSingle::evalSinceDirtyAttr).asBool())
		m_locator->computeCurveData(dataBlock);
	else if (dataBlock.outputValue(FlexiChainSingle::drawSinceEvalAttr).asBool())
		updateBuffers = false;

	// We will check if the items need to be updated
	bool itemsChanged = false;

	// Create render items if they do not yet exist
	if (!m_surfaceRenderItem || !m_borderActiveRenderItem || !m_borderDormantRenderItem || !m_curveActiveRenderItem || !m_curveDormantRenderItem ||
		!m_normalsActiveRenderItem || !m_normalsDormantRenderItem || !m_hullRenderItem || !m_boundingBoxRenderItem)
	{
		container.clear();

		// surface
		m_surfaceRenderItem = MHWRender::MRenderItem::Create(m_surfaceRenderItemName, MHWRender::MRenderItem::MaterialSceneItem, MHWRender::MGeometry::kTriangles);
		m_surfaceRenderItem->setDrawMode((MHWRender::MGeometry::DrawMode)(MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_surfaceRenderItem->depthPriority(MHWRender::MRenderItem::sDormantFilledDepthPriority); // Lowest priority
		container.add(m_surfaceRenderItem);

		// border active
		m_borderActiveRenderItem = MHWRender::MRenderItem::Create(m_borderActiveRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_borderActiveRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_borderActiveRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority); // Highest priority
		container.add(m_borderActiveRenderItem);

		// border dormant
		m_borderDormantRenderItem = MHWRender::MRenderItem::Create(m_borderDormantRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_borderDormantRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_borderDormantRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_borderDormantRenderItem);

		// curve active
		m_curveActiveRenderItem = MHWRender::MRenderItem::Create(m_curveActiveRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_curveActiveRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_curveActiveRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_curveActiveRenderItem);

		// curve dormant
		m_curveDormantRenderItem = MHWRender::MRenderItem::Create(m_curveDormantRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_curveDormantRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_curveDormantRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_curveDormantRenderItem);

		// normals active
		m_normalsActiveRenderItem = MHWRender::MRenderItem::Create(m_normalsActiveRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_normalsActiveRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_normalsActiveRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_normalsActiveRenderItem);

		// normals dormant
		m_normalsDormantRenderItem = MHWRender::MRenderItem::Create(m_normalsDormantRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_normalsDormantRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_normalsDormantRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_normalsDormantRenderItem);

		// hull (active + dormant)
		m_hullRenderItem = MHWRender::MRenderItem::Create(m_hullRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_hullRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_hullRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		m_hullRenderItem->setSelectionMask(MSelectionMask()); // Make unselectable
		container.add(m_hullRenderItem);

		// bounding box active
		// We are using the same technique Maya uses for default locators
		// - We are not implementing a NonMaterialSceneItem for kBoundingBox draw mode (Shading -> Bounding Box)
		// - Instead we are just supporting draw overrides (overrideLevelOfDetail plug)
		m_boundingBoxRenderItem = MHWRender::MRenderItem::Create(m_boundingBoxRenderItemName, MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines);
		m_boundingBoxRenderItem->setDrawMode((MGeometry::DrawMode)(MHWRender::MGeometry::kWireframe | MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured));
		m_boundingBoxRenderItem->depthPriority(MHWRender::MRenderItem::sSelectionDepthPriority);
		container.add(m_boundingBoxRenderItem);

		itemsChanged = true;
	}
	
	// Create shaders if they do not yet exist
	if (!m_wireframeShader || !m_surfaceShader || !m_borderDormantShader || !m_curveDormantShader || !m_normalsDormantShader || !m_hullShader)
	{
		MRenderer* renderer = MRenderer::theRenderer();
		if (!renderer)
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to get Viewport 2.0 renderer" << std::endl;
			return;
		}

		const MShaderManager* shaderManager = renderer->getShaderManager();
		if (!shaderManager)
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to get shader manager" << std::endl;
			return;
		}

		MFragmentManager* fragmentManager = renderer->getFragmentManager();
		if (!fragmentManager)
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to get fragment manager" << std::endl;
			return;
		}
		
		// We will maintain pointers to the shader instances so we can reuse them (deleted when destructor is called)
		// The reference to the underlying shader will be released when the shader instances are deleted
		createShaderInstance(shaderManager, fragmentManager);
		m_wireframeShader = shaderManager->getStockShader(MShaderManager::k3dThickLineShader);
		m_borderDormantShader = shaderManager->getStockShader(MShaderManager::k3dThickLineShader);
		m_curveDormantShader = shaderManager->getStockShader(MShaderManager::k3dThickLineShader);
		m_normalsDormantShader = shaderManager->getStockShader(MShaderManager::k3dThickLineShader);
		m_hullShader = shaderManager->getStockShader(MShaderManager::k3dThickLineShader);
		
		m_surfaceRenderItem->setShader(m_surfaceShader);
		m_borderActiveRenderItem->setShader(m_wireframeShader);
		m_borderDormantRenderItem->setShader(m_borderDormantShader);
		m_curveActiveRenderItem->setShader(m_wireframeShader);
		m_curveDormantRenderItem->setShader(m_curveDormantShader);
		m_normalsActiveRenderItem->setShader(m_wireframeShader);
		m_normalsDormantRenderItem->setShader(m_normalsDormantShader);
		m_hullRenderItem->setShader(m_hullShader);
		m_boundingBoxRenderItem->setShader(m_wireframeShader);
		
		itemsChanged = true;
	}

	// Create texture if it does not yet exist
	if (!m_rampTexture || !m_diffuseTexture || !m_samplerState)
	{
		MRenderer* renderer = MRenderer::theRenderer();
		if (!renderer)
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to get Viewport 2.0 renderer" << std::endl;
			return;
		}

		MHWRender::MTextureManager* textureManager = renderer->getTextureManager();
		if (!textureManager)
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to get texture manager" << std::endl;
			return;
		}

		// Retrieve resolved names
		if (!m_resolvedMapName.length())
		{
			const MHWRender::MAttributeParameterMapping* mapping = m_mappings.findByParameterName("map");
			if (mapping)
				m_resolvedMapName = mapping->resolvedParameterName();
		}

		if (!m_resolvedSamplerName.length())
		{
			const MHWRender::MAttributeParameterMapping* mapping = m_mappings.findByParameterName("mapSampler");
			if (mapping)
				m_resolvedSamplerName = mapping->resolvedParameterName();
		}

		if (!m_resolvedMapName.length() || !m_resolvedSamplerName.length())
		{
			std::cerr << "FlexiChainSingle_SubSceneOverride: Failed to retrieve resolved parameter names from fragment graph" << std::endl;
			return;
		}

		// Create sampler and set to linear-wrap
		MHWRender::MSamplerStateDesc samplerDesc;
		samplerDesc.filter = MHWRender::MSamplerState::kAnisotropic;
		samplerDesc.maxAnisotropy = 16;
		m_samplerState = MHWRender::MStateManager::acquireSamplerState(samplerDesc);
		m_surfaceShader->setParameter(m_resolvedSamplerName, *m_samplerState);

		// Acquire textures
		const MString rampTextureName{ "" };
		MHWRender::MTextureDescription rampTextureDesc;
		rampTextureDesc.setToDefault2DTexture();
		rampTextureDesc.fWidth = m_texturedPixelCount;
		rampTextureDesc.fHeight = 1;
		rampTextureDesc.fFormat = MHWRender::kR8G8B8A8_UNORM;
		m_rampTexture = textureManager->acquireTexture(rampTextureName, rampTextureDesc, m_texturedPixels, false);

		const MString diffuseTextureName{ "" };
		MHWRender::MTextureDescription diffuseTextureDesc;
		diffuseTextureDesc.setToDefault2DTexture();
		diffuseTextureDesc.fWidth = 1;
		diffuseTextureDesc.fHeight = 1;
		diffuseTextureDesc.fFormat = MHWRender::kR8G8B8A8_UNORM;
		m_diffuseTexture = textureManager->acquireTexture(diffuseTextureName, diffuseTextureDesc, m_diffusePixel, false);

		// Set the entire diffuse texture to a single color
		MImage image;
		for (int i = 0; i < 4; i++)
			m_diffusePixel[i] = (char)(m_surfaceColor[i] * 255);
		image.setPixels(m_diffusePixel, 1, 1);
		m_diffuseTexture->update(image, false);

		itemsChanged = true;
	}

	// Retrieve states that affect visibility
	// Certain values which do not affect evaluation have not been cached and need to be retrieved from the data block
	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	bool isVisible = path.isVisible();
	bool computeOrientation = curveData.isOrientEnabled;
	bool ribbonDrawEnabled = curveData.isDrawRibbonEnabled;
	bool curveDrawEnabled = dataBlock.inputValue(FlexiChainSingle::drawCurveAttr).asBool();
	bool normalsDrawEnabled = dataBlock.inputValue(FlexiChainSingle::drawNormalsAttr).asBool();
	bool hullDrawEnabled = dataBlock.inputValue(FlexiChainSingle::drawHullAttr).asBool();
	bool boundingBoxEnabled = (bool)m_overrideLevelOfDetailPlug.asShort();
	bool scaleAdjustmentsEnabled = curveData.isScaleAdjustmentEnabled;
	bool twistAdjustmentsEnabled = curveData.isTwistAdjustmentEnabled;

	// Retrieve shader states/properties
	MHWRender::DisplayStatus displayStatus = MHWRender::MGeometryUtilities::displayStatus(path);
	MColor wireframeActiveColor = MHWRender::MGeometryUtilities::wireframeColor(path);
	bool castsShadows = m_castsShadowsPlug.asBool();
	bool receiveShadows = m_receiveShadowsPlug.asBool();
	MString context = MFnToolContext(MGlobal::currentToolContext()).name();
	bool upVectorContextEnabled = context == "FlexiChainSingleUpVectorContext1" ? true : false;
	bool scaleAdjustmentContextEnabled = context == "FlexiChainSingleScaleAdjustmentContext1" && scaleAdjustmentsEnabled ? true : false;
	bool twistAdjustmentContextEnabled = context == "FlexiChainSingleTwistAdjustmentContext1" && twistAdjustmentsEnabled ? true : false;

	// Update internal states
	if (displayStatus != m_displayStatus || wireframeActiveColor != m_wireframeColor || castsShadows != m_castsShadows || receiveShadows != m_receiveShadows ||
		isVisible != m_isVisible || computeOrientation != m_computeOrientation || ribbonDrawEnabled != m_ribbonDrawEnabled || curveDrawEnabled != m_curveDrawEnabled || 
		normalsDrawEnabled != m_normalsDrawEnabled || hullDrawEnabled != m_hullDrawEnabled || boundingBoxEnabled != m_boundingBoxEnabled ||
		upVectorContextEnabled != m_upVectorContextEnabled || scaleAdjustmentContextEnabled != m_scaleAdjustmentContextEnabled ||
		twistAdjustmentContextEnabled != m_twistAdjustmentContextEnabled)
	{
		m_displayStatus = displayStatus;
		m_wireframeColor = wireframeActiveColor;
		m_castsShadows = castsShadows;
		m_receiveShadows = receiveShadows;
		m_isVisible = isVisible;
		m_computeOrientation = computeOrientation;
		m_ribbonDrawEnabled = ribbonDrawEnabled;
		m_curveDrawEnabled = curveDrawEnabled;
		m_normalsDrawEnabled = normalsDrawEnabled;
		m_hullDrawEnabled = hullDrawEnabled;
		m_boundingBoxEnabled = boundingBoxEnabled;
		m_upVectorContextEnabled = upVectorContextEnabled;
		m_scaleAdjustmentContextEnabled = scaleAdjustmentContextEnabled;
		m_twistAdjustmentContextEnabled = twistAdjustmentContextEnabled;

		itemsChanged = true;
	}

	if (itemsChanged)
	{
		// Update render items
		bool activeItemDrawEnabled = m_displayStatus == MHWRender::kLead || m_displayStatus == MHWRender::kActive || m_displayStatus == MHWRender::kHilite;
		bool dormantItemDrawEnabled = m_displayStatus == MHWRender::kDormant;

		bool surfaceItemEnabled = m_isVisible && m_computeOrientation && m_ribbonDrawEnabled && !m_boundingBoxEnabled;
		m_surfaceRenderItem->enable(surfaceItemEnabled);
		m_surfaceRenderItem->castsShadows(m_castsShadows);
		m_surfaceRenderItem->receivesShadows(m_receiveShadows);

		bool borderActiveItemEnabled = m_isVisible && m_computeOrientation && m_ribbonDrawEnabled && activeItemDrawEnabled && !m_boundingBoxEnabled;
		m_borderActiveRenderItem->enable(borderActiveItemEnabled);

		bool borderDormantItemEnabled = m_isVisible && m_computeOrientation && m_ribbonDrawEnabled && dormantItemDrawEnabled && !m_boundingBoxEnabled;
		m_borderDormantRenderItem->enable(borderDormantItemEnabled);

		bool curveActiveItemEnabled = m_isVisible && m_curveDrawEnabled && activeItemDrawEnabled && !m_boundingBoxEnabled;
		m_curveActiveRenderItem->enable(curveActiveItemEnabled);

		bool curveDormantItemEnabled = m_isVisible && m_curveDrawEnabled && dormantItemDrawEnabled && !m_boundingBoxEnabled;
		m_curveDormantRenderItem->enable(curveDormantItemEnabled);

		bool normalsActiveItemEnabled = m_isVisible && m_computeOrientation && m_normalsDrawEnabled && activeItemDrawEnabled && !m_boundingBoxEnabled;
		m_normalsActiveRenderItem->enable(normalsActiveItemEnabled);

		bool normalsDormantItemEnabled = m_isVisible && m_computeOrientation && m_normalsDrawEnabled && dormantItemDrawEnabled && !m_boundingBoxEnabled;
		m_normalsDormantRenderItem->enable(normalsDormantItemEnabled);

		bool hullItemEnabled = m_isVisible && m_hullDrawEnabled && !m_boundingBoxEnabled;
		m_hullRenderItem->enable(hullItemEnabled);

		bool boundingBoxItemEnabled = m_isVisible && m_boundingBoxEnabled;
		m_boundingBoxRenderItem->enable(boundingBoxItemEnabled);

		// Update line shaders
		updateLineShader(m_wireframeShader, "lineWidth", 1.1f, "solidColor", m_wireframeColor);
		updateLineShader(m_borderDormantShader, "lineWidth", 1.1f, "solidColor", m_borderDormantColor);
		updateLineShader(m_curveDormantShader, "lineWidth", 1.1f, "solidColor", m_curveDormantColor);
		updateLineShader(m_normalsDormantShader, "lineWidth", 1.8f, "solidColor", m_normalsDormantColor);
		updateLineShader(m_hullShader, "lineWidth", 0.6f, "solidColor", m_curveDormantColor);

		// Update surface shader
		MHWRender::MTextureAssignment textureAssignment;
		if (m_upVectorContextEnabled || m_scaleAdjustmentContextEnabled || m_twistAdjustmentContextEnabled)
			textureAssignment.texture = m_rampTexture;
		else
			textureAssignment.texture = m_diffuseTexture;

		m_surfaceShader->setParameter(m_resolvedMapName, textureAssignment);
	}

	// We need to set the matrix for any manually created render items as they are not automatically associated with the DAG node
	// This can be tested by calling MRenderItem::sourceDagPath()
	MMatrix drawTransform;
	short drawSpaceTransformation = dataBlock.inputValue(FlexiChainSingle::drawSpaceTransformationAttr).asShort();

	if (drawSpaceTransformation == 1)
		drawTransform = path.inclusiveMatrix();
	else if (drawSpaceTransformation == 2)
		drawTransform = dataBlock.inputValue(FlexiChainSingle::customDrawSpaceTransformAttr).asMatrix();

	if (!m_drawTransform.isEquivalent(drawTransform))
	{
		m_surfaceRenderItem->setMatrix(&drawTransform);
		m_borderActiveRenderItem->setMatrix(&drawTransform);
		m_borderDormantRenderItem->setMatrix(&drawTransform);
		m_curveActiveRenderItem->setMatrix(&drawTransform);
		m_curveDormantRenderItem->setMatrix(&drawTransform);
		m_normalsActiveRenderItem->setMatrix(&drawTransform);
		m_normalsDormantRenderItem->setMatrix(&drawTransform);
		m_hullRenderItem->setMatrix(&drawTransform);
		m_boundingBoxRenderItem->setMatrix(&drawTransform);

		m_drawTransform = drawTransform;
	}

	// Return if buffers have already been updated since the last evaluation
	// Unless an item has changed, as items to not update whilst disabled therefore their buffers may be stale when reenabled
	if (!updateBuffers && !itemsChanged)
	{
		// Selecting manipulators within the up-vector context does not cause any dirty propagation in the DG however it should affect how the ramp texture is drawn
		// Therefore, if the up-vector context is active we must force the texture to update before returning (this must be done on all draw cycles)
		// Performance is not a major concern whilst the context is active (ie. heavy animation work is not done inside the context)
		if (m_isVisible && !m_boundingBoxEnabled && m_ribbonDrawEnabled && m_computeOrientation)
		{
			if (m_upVectorContextEnabled)
				updateRampTextureUpVectorContext();
			else if (m_scaleAdjustmentContextEnabled)
				updateRampTextureScaleAdjustmentContext();
			else if (m_twistAdjustmentContextEnabled)
				updateRampTextureTwistAdjustmentContext();
		}

		return;
	}

	if (m_isVisible)
	{
		const MBoundingBox bounds = m_locator->boundingBox();
		
		// Bounding box buffers
		if (m_boundingBoxEnabled)
		{
			updateBoundingBoxGeometryBuffers(bounds);

			MHWRender::MVertexBufferArray boundingBoxVertexBuffers;
			boundingBoxVertexBuffers.addBuffer("positions", m_boundingBoxPositionBuffer);
			setGeometryForRenderItem(*m_boundingBoxRenderItem, boundingBoxVertexBuffers, *m_boundingBoxIndexBuffer, &bounds);
		}
		else
		{
			// Ribbon (surface + border) buffers and texture fragment
			if (m_ribbonDrawEnabled && m_computeOrientation)
			{
				updateRibbonGeometryBuffers();
				if (m_upVectorContextEnabled)
					updateRampTextureUpVectorContext();
				else if (m_scaleAdjustmentContextEnabled)
					updateRampTextureScaleAdjustmentContext();
				else if (m_twistAdjustmentContextEnabled)
					updateRampTextureTwistAdjustmentContext();

				MHWRender::MVertexBufferArray surfaceVertexBuffers;
				surfaceVertexBuffers.addBuffer("positions", m_surfacePositionBuffer);
				surfaceVertexBuffers.addBuffer("normals", m_surfaceNormalBuffer);
				surfaceVertexBuffers.addBuffer("uvs", m_surfaceTextureBuffer);
				setGeometryForRenderItem(*m_surfaceRenderItem, surfaceVertexBuffers, *m_surfaceIndexBuffer, &bounds);

				MHWRender::MVertexBufferArray borderVertexBuffers;
				borderVertexBuffers.addBuffer("positions", m_surfacePositionBuffer);
				setGeometryForRenderItem(*m_borderActiveRenderItem, borderVertexBuffers, *m_borderIndexBuffer, &bounds);
				setGeometryForRenderItem(*m_borderDormantRenderItem, borderVertexBuffers, *m_borderIndexBuffer, &bounds);
			}

			// Curve buffers
			if (m_curveDrawEnabled)
			{
				updateCurveGeometryBuffers();

				MHWRender::MVertexBufferArray curveVertexBuffers;
				curveVertexBuffers.addBuffer("positions", m_curvePositionBuffer);
				setGeometryForRenderItem(*m_curveActiveRenderItem, curveVertexBuffers, *m_curveIndexBuffer, &bounds);
				setGeometryForRenderItem(*m_curveDormantRenderItem, curveVertexBuffers, *m_curveIndexBuffer, &bounds);
			}

			// Normals buffers
			if (m_normalsDrawEnabled && m_computeOrientation)
			{
				updateNormalsGeometryBuffers();

				MHWRender::MVertexBufferArray normalsVertexBuffers;
				normalsVertexBuffers.addBuffer("positions", m_normalsPositionBuffer);
				setGeometryForRenderItem(*m_normalsActiveRenderItem, normalsVertexBuffers, *m_normalsIndexBuffer, &bounds);
				setGeometryForRenderItem(*m_normalsDormantRenderItem, normalsVertexBuffers, *m_normalsIndexBuffer, &bounds);
			}

			// Hull buffers
			if (m_hullDrawEnabled)
			{
				updateHullGeometryBuffers();

				MHWRender::MVertexBufferArray hullVertexBuffers;
				hullVertexBuffers.addBuffer("positions", m_hullPositionBuffer);
				setGeometryForRenderItem(*m_hullRenderItem, hullVertexBuffers, *m_hullIndexBuffer, &bounds);
			}
		}
	}

	dataBlock.outputValue(FlexiChainSingle::drawSinceEvalAttr).setBool(true);
}

// ------ Helpers ------

/*	Description
	-----------
	Creates and caches a Lambert surface shader fragment as a MShaderInstance
	Creates a 2D texture sampling fragment and connects its output to the Lambert surface shader "color" parameter
	- The "color" parameter is a float3 input therefore our custom fragment outputs float3 rgb values

	Notes
	-----
	Maya provides a number of internal hardware fragments which can be accessed via the MFragmentManager
	The "mayaRamp" fragment graph provides the fragment code that is used by Maya's ramp node to sample a ramp texture
	However the actual texture code is most likely written in a MPxShadingNodeOverride associated with the ramp node (not available to us)
	The "mayaRamp" fragment graph is therefore not suited for this node as it mainly just provides extra sampling functionality (eg. noise sampling)
	We just want a basic 2D texture sampler which we will use to sample our custom texture    */
void FlexiChainSingle_SubSceneOverride::createShaderInstance(const MShaderManager* shaderManager, MFragmentManager* fragmentManager)
{
	// Create shader
	m_surfaceShader = shaderManager->getFragmentShader("mayaLambertSurface", "outSurfaceFinal", true);

	// Fragment was registered by plugin
	assert(fragmentManager->hasFragment("mrsTexture2D"));

	// Set up mappings for the texture fragment input parameters
	// This will let us retrieve the resolved names if the they are renamed upon Maya translating the shading network
	// We leave the attribute name empty as there is no corresponding attribute on the node / we are not implementing a MPxShadingNodeOverride
	MHWRender::MAttributeParameterMapping mapMapping("map", "", false, true);
	MHWRender::MAttributeParameterMapping mapSamplerMapping("mapSampler", "", false, true);
	m_mappings.append(mapMapping);
	m_mappings.append(mapSamplerMapping);

	// Add our custom fragment as an input
	m_surfaceShader->addInputFragment("mrsTexture2D", "output", "color");
}

/*	Description
	-----------
	Updates the ramp so that every pixel has a linear space mid grey value (will be lighter in color managed viewport)
	Usually called in case of an error during the texture update process or when no texturing is required    */
void FlexiChainSingle_SubSceneOverride::setDefaultRamp()
{
	unsigned int pointerOffset = 0;
	for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount; ++pixelIndex)
	{
		m_texturedPixels[pointerOffset++] = 128;
		m_texturedPixels[pointerOffset++] = 128;
		m_texturedPixels[pointerOffset++] = 128;
		m_texturedPixels[pointerOffset++] = 255;
	}

	MImage image;
	image.setPixels(m_texturedPixels, m_texturedPixelCount, 1);
	m_rampTexture->update(image, false);
}

/*	Description
	-----------
	Updates the ramp texture corresponding to the up-vector stability data for the current locator node
	The texture is used as an input for the texture sampling fragment
	This function will only be called if the current context is set to FlexiChainSingleUpVectorContext1
	The ramp will only be displayed if one of the two manipulators belonging to the locator are active
	- If the start manipulator is active, the ramp will represent the stability of the curves principal normal in relation to its respective up-vector
	- If the end manipulator is active, the ramp will represent the stability of the curves counter-twist data in relation to its respective up-vector    */
void FlexiChainSingle_SubSceneOverride::updateRampTextureUpVectorContext()
{
	MStatus status;

	// Check if one of the manipulators belonging to the locator is active
	MString activeManipPlugCmd = "FlexiChainSingleUpVectorContext -q -activeManipPlug FlexiChainSingleUpVectorContext1";
	MString activeManipPlugName = MGlobal::executeCommandStringResult(activeManipPlugCmd);
	MPlug activeManipPlug;
	MSelectionList selList;
	status = selList.add(activeManipPlugName);
	if (status)
		status = selList.getPlug(0, activeManipPlug);

	// If there are no active manipulators, set the entire ramp to a uniform color
	if (!status)
	{
		setDefaultRamp();
		return;
	}

	// Constants
	double lowThreshold = 0.8;
	double maxIntensity = 0.9;
	double influenceRatio = 0.25;
	unsigned int influencedPixelCount = (int)(m_texturedPixelCount * influenceRatio);

	// Calculate pixel data
	if (activeManipPlug == FlexiChainSingle::normalUpVectorOverrideCompoundAttr)
	{
		double stability;
		m_locator->computeNormalStability(stability);
		stability = std::abs(stability);
		unsigned int pointerOffset = 0;
		
		if (stability > lowThreshold)
		{
			// Remap = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
			double weightedStability = (stability - 0.8) / 0.2;

			for (unsigned int pixelIndex = 0; pixelIndex < influencedPixelCount; ++pixelIndex)
			{
				// Initial pixels receive highest weight
				double pixelWeight = (double)(influencedPixelCount - 1 - pixelIndex) / (influencedPixelCount - 1) * weightedStability;
				pixelWeight = pixelWeight * pixelWeight * (3 - 2 * pixelWeight);

				char rChannel = (char)((pixelWeight * 255) + (1.0 - pixelWeight) * 128);
				// Max intensity simply reduces the intensity of the red color
				char bgChannel = (char)((1.0 - pixelWeight * maxIntensity) * 128);

				m_texturedPixels[pointerOffset++] = rChannel;
				m_texturedPixels[pointerOffset++] = bgChannel;
				m_texturedPixels[pointerOffset++] = bgChannel;
				m_texturedPixels[pointerOffset++] = 255;
			}

			for (unsigned int pixelIndex = influencedPixelCount; pixelIndex < m_texturedPixelCount; ++pixelIndex)
			{
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 255;
			}
		}
		else
		{
			for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount; ++pixelIndex)
			{
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 255;
			}
		}
	}
	else
	{
		double stability;
		m_locator->computeCounterTwistStability(stability);
		stability = std::abs(stability);
		unsigned int pointerOffset = 0;

		if (stability > lowThreshold)
		{
			double weightedStability = (stability - 0.8) / 0.2;

			for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount - influencedPixelCount; ++pixelIndex)
			{
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 255;
			}

			for (unsigned int pixelIndex = m_texturedPixelCount - influencedPixelCount; pixelIndex < m_texturedPixelCount; ++pixelIndex)
			{
				// End pixels receive highest weight
				double pixelWeight = (double)(pixelIndex - m_texturedPixelCount + influencedPixelCount) / (influencedPixelCount - 1) * weightedStability;
				pixelWeight = pixelWeight * pixelWeight * (3 - 2 * pixelWeight);

				char rChannel = (char)((pixelWeight * 255) + (1.0 - pixelWeight) * 128);
				char bgChannel = (char)((1.0 - pixelWeight * maxIntensity) * 128);

				m_texturedPixels[pointerOffset++] = rChannel;
				m_texturedPixels[pointerOffset++] = bgChannel;
				m_texturedPixels[pointerOffset++] = bgChannel;
				m_texturedPixels[pointerOffset++] = 255;
			}
		}
		else
		{
			for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount; ++pixelIndex)
			{
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 128;
				m_texturedPixels[pointerOffset++] = 255;
			}
		}
	}

	// Create image for texture
	MImage image;
	image.setPixels(m_texturedPixels, m_texturedPixelCount, 1);
	m_rampTexture->update(image, false);
}

void FlexiChainSingle_SubSceneOverride::updateRampTextureScaleAdjustmentContext()
{
	MStatus status;

	// Check if one of the manipulators belonging to the locator is active
	MString activeManipPlugCmd = "FlexiChainSingleScaleAdjustmentContext -q -activeManipPlug FlexiChainSingleScaleAdjustmentContext1";
	MString activeManipPlugName = MGlobal::executeCommandStringResult(activeManipPlugCmd);
	MPlug activeManipPlug;
	MSelectionList selList;
	status = selList.add(activeManipPlugName);
	if (status)
		status = selList.getPlug(0, activeManipPlug);

	// If there are no active manipulators, set the entire ramp to a uniform color
	if (!status)
	{
		setDefaultRamp();
		return;
	}

	// Retrieve the correct falloff ramp
	MIntArray logicalIndices;
	activeManipPlug.array().getExistingArrayAttributeIndices(logicalIndices);
	unsigned int logicalIndex = activeManipPlug.logicalIndex();

	int physicalIndex = -1;
	for (unsigned int i = 0; i < logicalIndices.length(); ++i)
	{
		if (logicalIndices[i] == logicalIndex)
			physicalIndex = i;
	}
	if (physicalIndex == -1)
	{
		setDefaultRamp();
		return;
	}

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	assert(physicalIndex < curveData.scaleAdjustments.size());
	ScaleAdjustment scaleAdjustmentRamp = curveData.scaleAdjustments[physicalIndex];

	// Constants
	double maxIntensity = 0.9;

	// Calculate pixel data
	unsigned int pointerOffset = 0;
	for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount; ++pixelIndex)
	{
		double falloffParam = (double)pixelIndex / (m_texturedPixelCount - 1);
		double pixelWeight = scaleAdjustmentRamp.curve.getValue(falloffParam);

		char rChannel = (char)((pixelWeight * 255) + (1.0 - pixelWeight) * 128);
		// Max intensity simply reduces the intensity of the red color
		char bgChannel = (char)((1.0 - pixelWeight * maxIntensity) * 128);

		m_texturedPixels[pointerOffset++] = rChannel;
		m_texturedPixels[pointerOffset++] = bgChannel;
		m_texturedPixels[pointerOffset++] = bgChannel;
		m_texturedPixels[pointerOffset++] = 255;
	}

	// Create image for texture
	MImage image;
	image.setPixels(m_texturedPixels, m_texturedPixelCount, 1);
	m_rampTexture->update(image, false);
}

void FlexiChainSingle_SubSceneOverride::updateRampTextureTwistAdjustmentContext()
{
	MStatus status;

	// Check if one of the manipulators belonging to the locator is active
	MString activeManipPlugCmd = "FlexiChainSingleTwistAdjustmentContext -q -activeManipPlug FlexiChainSingleTwistAdjustmentContext1";
	MString activeManipPlugName = MGlobal::executeCommandStringResult(activeManipPlugCmd);
	MPlug activeManipPlug;
	MSelectionList selList;
	status = selList.add(activeManipPlugName);
	if (status)
		status = selList.getPlug(0, activeManipPlug);

	// If there are no active manipulators, set the entire ramp to a uniform color
	if (!status)
	{
		setDefaultRamp();
		return;
	}

	// Retrieve the correct falloff ramp
	MIntArray logicalIndices;
	activeManipPlug.array().getExistingArrayAttributeIndices(logicalIndices);
	unsigned int logicalIndex = activeManipPlug.logicalIndex();
	int physicalIndex = -1;
	for (unsigned int i = 0; i < logicalIndices.length(); ++i)
	{
		if (logicalIndices[i] == logicalIndex)
			physicalIndex = i;
	}
	if (physicalIndex == -1)
	{
		setDefaultRamp();
		return;
	}

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	assert(physicalIndex < curveData.twistAdjustments.size());
	TwistAdjustment twistAdjustmentRamp = curveData.twistAdjustments[physicalIndex];

	// Constants
	double maxIntensity = 0.9;

	// Calculate pixel data
	unsigned int pointerOffset = 0;
	for (unsigned int pixelIndex = 0; pixelIndex < m_texturedPixelCount; ++pixelIndex)
	{
		double falloffParam = (double)pixelIndex / (m_texturedPixelCount - 1);
		double pixelWeight = twistAdjustmentRamp.curve.getValue(falloffParam);

		char rChannel = (char)((pixelWeight * 255) + (1.0 - pixelWeight) * 128);
		// Max intensity simply reduces the intensity of the red color
		char bgChannel = (char)((1.0 - pixelWeight * maxIntensity) * 128);

		m_texturedPixels[pointerOffset++] = rChannel;
		m_texturedPixels[pointerOffset++] = bgChannel;
		m_texturedPixels[pointerOffset++] = bgChannel;
		m_texturedPixels[pointerOffset++] = 255;
	}

	// Create image for texture
	MImage image;
	image.setPixels(m_texturedPixels, m_texturedPixelCount, 1);
	m_rampTexture->update(image, false);
}

void FlexiChainSingle_SubSceneOverride::updateSurfaceShader(MShaderInstance* shader, const char* colorParameter, MColor color)
{
	shader->setParameter(colorParameter, &color.r);
}

void FlexiChainSingle_SubSceneOverride::updateLineShader(MShaderInstance* shader, const char* lineWidthParameter, float lineWidth, const char* colorParameter, MColor color)
{
	shader->setParameter(colorParameter, &color.r);
	// Not sure what first number does
	float lineWidthArray[2] = { lineWidth, lineWidth };
	shader->setParameter(lineWidthParameter, lineWidthArray);
}

// Ribbon buffers
void FlexiChainSingle_SubSceneOverride::updateRibbonGeometryBuffers()
{
	deleteRibbonVertexBuffers();

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	unsigned int curveVertexCount = curveData.sampleCount;
	unsigned int surfaceVertexCount = curveVertexCount * 2;
	unsigned int surfaceIndicesCount = (curveVertexCount - 1) * 6;
	unsigned int borderIndicesCount = curveVertexCount * 4;

	// Acquire vertex buffer resources
	const MVertexBufferDescriptor surfacePositionDesc("", MGeometry::kPosition, MGeometry::kFloat, 3);
	const MVertexBufferDescriptor surfaceNormalDesc("", MGeometry::kNormal, MGeometry::kFloat, 3);
	const MVertexBufferDescriptor surfaceTextureDesc("", MGeometry::kTexture, MGeometry::kFloat, 2);
	m_surfacePositionBuffer = new MVertexBuffer(surfacePositionDesc);
	m_surfaceNormalBuffer = new MVertexBuffer(surfaceNormalDesc);
	m_surfaceTextureBuffer = new MVertexBuffer(surfaceTextureDesc);

	// VB for surface positions
	if (m_surfacePositionBuffer)
	{
		// Return a block of memory to fill with our position data (bytes = size * desc.dataTypeSize() * desc.dimension())
		float* surfacePositions = (float*)m_surfacePositionBuffer->acquire(surfaceVertexCount, true /*writeOnly*/);
		if (surfacePositions)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount; i++)
			{
				MFloatVector vert = curveData.points[i] + curveData.binormals[i] * m_ribbonWidthScale;
				MFloatVector vertOpposite = curveData.points[i] + curveData.binormals[i] * m_ribbonWidthScale * -1;

				surfacePositions[pointerOffset++] = vert[0];
				surfacePositions[pointerOffset++] = vert[1];
				surfacePositions[pointerOffset++] = vert[2];
				surfacePositions[pointerOffset++] = vertOpposite[0];
				surfacePositions[pointerOffset++] = vertOpposite[1];
				surfacePositions[pointerOffset++] = vertOpposite[2];
			}

			// Transfer from CPU to GPU memory
			m_surfacePositionBuffer->commit(surfacePositions);
		}
	}

	// VB for surface normals
	if (m_surfaceNormalBuffer)
	{
		float* surfaceNormals = (float*)m_surfaceNormalBuffer->acquire(surfaceVertexCount, true /*writeOnly*/);
		if (surfaceNormals)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount; i++)
			{
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][0];
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][1];
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][2];
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][0];
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][1];
				surfaceNormals[pointerOffset++] = (float)curveData.normals[i][2];
			}

			m_surfaceNormalBuffer->commit(surfaceNormals);
		}
	}

	// VB for surface textures
	if (m_surfaceTextureBuffer)
	{
		float* surfaceTextures = (float*)m_surfaceTextureBuffer->acquire(surfaceVertexCount, true /*writeOnly*/);
		if (surfaceTextures)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount; i++)
			{
				float xCoord = (float)i / (curveVertexCount - 1);
				surfaceTextures[pointerOffset++] = xCoord;
				surfaceTextures[pointerOffset++] = 1.0f;
				surfaceTextures[pointerOffset++] = xCoord;
				surfaceTextures[pointerOffset++] = 0.0f;
			}
			
			// The texture will fill a 1*1 UV patch
			// Ensure our UVs do not reach all the way to the edge of this patch as this was causing a bleeding effect on the first and last pixels
			surfaceTextures[0] = 0.001f;
			surfaceTextures[2] = 0.001f;
			surfaceTextures[pointerOffset - 4] = 0.999f;
			surfaceTextures[pointerOffset - 2] = 0.999f;

			m_surfaceTextureBuffer->commit(surfaceTextures);
		}
	}

	// Index buffers only need to be updated if the sampleCount attribute changes
	if (surfaceVertexCount == m_surfaceVertexCount)
		return;

	m_surfaceVertexCount = surfaceVertexCount;

	deleteRibbonIndexBuffers();
	m_surfaceIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);
	m_borderIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

	// IB for surface item
	if (m_surfaceIndexBuffer)
	{
		unsigned int* surfaceIndices = (unsigned int*)m_surfaceIndexBuffer->acquire(surfaceIndicesCount, true /*writeOnly*/);
		if (surfaceIndices)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount - 1; i++)
			{
				// First triangle
				surfaceIndices[pointerOffset++] = i * 2;
				surfaceIndices[pointerOffset++] = i * 2 + 1;
				surfaceIndices[pointerOffset++] = i * 2 + 2;

				// Second triangle
				surfaceIndices[pointerOffset++] = i * 2 + 3;
				surfaceIndices[pointerOffset++] = i * 2 + 2;
				surfaceIndices[pointerOffset++] = i * 2 + 1;
			}

			m_surfaceIndexBuffer->commit(surfaceIndices);
		}
	}

	// IB for border item
	if (m_borderIndexBuffer)
	{
		unsigned int* borderIndices = (unsigned int*)m_borderIndexBuffer->acquire(borderIndicesCount, true /*writeOnly*/);
		if (borderIndices)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount - 1; i++)
			{
				// Top
				borderIndices[pointerOffset++] = i * 2;
				borderIndices[pointerOffset++] = i * 2 + 2;
				// Bottom
				borderIndices[pointerOffset++] = i * 2 + 1;
				borderIndices[pointerOffset++] = i * 2 + 3;
			}
			// Left
			borderIndices[pointerOffset++] = 0;
			borderIndices[pointerOffset++] = 1;
			// Right
			borderIndices[pointerOffset++] = (curveVertexCount - 1) * 2;
			borderIndices[pointerOffset++] = (curveVertexCount - 1) * 2 + 1;

			m_borderIndexBuffer->commit(borderIndices);
		}
	}
}

void FlexiChainSingle_SubSceneOverride::deleteRibbonVertexBuffers()
{
	if (m_surfacePositionBuffer)
	{
		delete m_surfacePositionBuffer;
		m_surfacePositionBuffer = nullptr;
	}

	if (m_surfaceNormalBuffer)
	{
		delete m_surfaceNormalBuffer;
		m_surfaceNormalBuffer = nullptr;
	}

	if (m_surfaceTextureBuffer)
	{
		delete m_surfaceTextureBuffer;
		m_surfaceTextureBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteRibbonIndexBuffers()
{
	if (m_surfaceIndexBuffer)
	{
		delete m_surfaceIndexBuffer;
		m_surfaceIndexBuffer = nullptr;
	}

	if (m_borderIndexBuffer)
	{
		delete m_borderIndexBuffer;
		m_borderIndexBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteRibbonGeometryBuffers()
{
	deleteRibbonVertexBuffers();
	deleteRibbonIndexBuffers();
}

// Curve buffers
void FlexiChainSingle_SubSceneOverride::updateCurveGeometryBuffers()
{
	deleteCurveVertexBuffers();

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	unsigned int curveVertexCount = curveData.sampleCount;
	unsigned int curveIndicesCount = (curveVertexCount % 2) ? curveVertexCount - 1 : curveVertexCount;

	// Acquire vertex buffer resources
	const MVertexBufferDescriptor curvePositionDesc("", MGeometry::kPosition, MGeometry::kFloat, 3);
	m_curvePositionBuffer = new MVertexBuffer(curvePositionDesc);

	// VB for curve positions
	if (m_curvePositionBuffer)
	{
		float* curvePositions = (float*)m_curvePositionBuffer->acquire(curveVertexCount, true /*writeOnly*/);
		if (curvePositions)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveVertexCount; i++)
			{
				curvePositions[pointerOffset++] = (float)curveData.points[i][0];
				curvePositions[pointerOffset++] = (float)curveData.points[i][1];
				curvePositions[pointerOffset++] = (float)curveData.points[i][2];
			}

			m_curvePositionBuffer->commit(curvePositions);
		}
	}

	// Index buffers only need to be updated if the sampleCount attribute changes
	if (curveVertexCount == m_curveVertexCount)
		return;

	m_curveVertexCount = curveVertexCount;

	deleteCurveIndexBuffers();
	m_curveIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

	// IB for curve item
	if (m_curveIndexBuffer)
	{
		unsigned int* curveIndices = (unsigned int*)m_curveIndexBuffer->acquire(curveIndicesCount, true /*writeOnly*/);
		if (curveIndices)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveIndicesCount; i++)
				curveIndices[pointerOffset++] = i;

			m_curveIndexBuffer->commit(curveIndices);
		}
	}
}

void FlexiChainSingle_SubSceneOverride::deleteCurveVertexBuffers()
{
	if (m_curvePositionBuffer)
	{
		delete m_curvePositionBuffer;
		m_curvePositionBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteCurveIndexBuffers()
{
	if (m_curveIndexBuffer)
	{
		delete m_curveIndexBuffer;
		m_curveIndexBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteCurveGeometryBuffers()
{
	deleteCurveVertexBuffers();
	deleteCurveIndexBuffers();
}

// Normals buffers
void FlexiChainSingle_SubSceneOverride::updateNormalsGeometryBuffers()
{
	deleteNormalsVertexBuffers();

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	unsigned int normalsVertexCount = curveData.outputCount * 2;
	unsigned int skipCount = (curveData.sampleCount - 1) / (curveData.outputCount - 1);

	// Acquire vertex buffer resources
	const MVertexBufferDescriptor normalsPositionDesc("", MGeometry::kPosition, MGeometry::kFloat, 3);
	m_normalsPositionBuffer = new MVertexBuffer(normalsPositionDesc);

	// VB for normals positions
	if (m_normalsPositionBuffer)
	{
		float* normalsPositions = (float*)m_normalsPositionBuffer->acquire(normalsVertexCount, true /*writeOnly*/);
		if (normalsPositions)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < curveData.outputCount; i++)
			{
				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][0];
				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][1];
				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][2];

				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][0] + (float)curveData.normals[i * skipCount][0] * m_normalsLengthScale;
				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][1] + (float)curveData.normals[i * skipCount][1] * m_normalsLengthScale;
				normalsPositions[pointerOffset++] = (float)curveData.points[i * skipCount][2] + (float)curveData.normals[i * skipCount][2] * m_normalsLengthScale;
			}

			m_normalsPositionBuffer->commit(normalsPositions);
		}
	}

	// Index buffers only need to be updated if the sampleCount attribute changes
	if (normalsVertexCount == m_normalsVertexCount)
		return;

	m_normalsVertexCount = normalsVertexCount;

	deleteNormalsIndexBuffers();
	m_normalsIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

	// IB for normals item
	if (m_normalsIndexBuffer)
	{
		unsigned int* normalsIndices = (unsigned int*)m_normalsIndexBuffer->acquire(normalsVertexCount, true /*writeOnly*/);
		if (normalsIndices)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < normalsVertexCount; i++)
				normalsIndices[pointerOffset++] = i;

			m_normalsIndexBuffer->commit(normalsIndices);
		}
	}
}

void FlexiChainSingle_SubSceneOverride::deleteNormalsVertexBuffers()
{
	if (m_normalsPositionBuffer)
	{
		delete m_normalsPositionBuffer;
		m_normalsPositionBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteNormalsIndexBuffers()
{
	if (m_normalsIndexBuffer)
	{
		delete m_normalsIndexBuffer;
		m_normalsIndexBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteNormalsGeometryBuffers()
{
	deleteNormalsVertexBuffers();
	deleteNormalsIndexBuffers();
}

// Hull buffers
void FlexiChainSingle_SubSceneOverride::updateHullGeometryBuffers()
{
	deleteHullVertexBuffers();

	const FlexiChainSingle::FlexiChainSingle_Data& curveData = m_locator->getCurveData();
	unsigned int hullVertexCount = 4;
	unsigned int hullIndicesCount = 6;

	// Acquire vertex buffer resources
	const MVertexBufferDescriptor hullPositionDesc("", MGeometry::kPosition, MGeometry::kFloat, 3);
	m_hullPositionBuffer = new MVertexBuffer(hullPositionDesc);

	// VB for hull positions
	if (m_hullPositionBuffer)
	{
		float* hullPositions = (float*)m_hullPositionBuffer->acquire(hullVertexCount, true /*writeOnly*/);
		if (hullPositions)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < hullVertexCount; i++)
			{
				hullPositions[pointerOffset++] = (float)curveData.controlPoints[i][0];
				hullPositions[pointerOffset++] = (float)curveData.controlPoints[i][1];
				hullPositions[pointerOffset++] = (float)curveData.controlPoints[i][2];
			}

			m_hullPositionBuffer->commit(hullPositions);
		}
	}

	// Index buffer is static once created
	if (m_hullIndexBuffer)
		return;

	m_hullIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

	// IB for hull item
	if (m_hullIndexBuffer)
	{
		unsigned int* hullIndices = (unsigned int*)m_hullIndexBuffer->acquire(hullIndicesCount, true /*writeOnly*/);
		if (hullIndices)
		{
			int pointerOffset = 0;

			for (unsigned int i = 0; i < hullVertexCount - 1; i++)
			{
				hullIndices[pointerOffset++] = i;
				hullIndices[pointerOffset++] = i + 1;
			}

			m_hullIndexBuffer->commit(hullIndices);
		}
	}
}

void FlexiChainSingle_SubSceneOverride::deleteHullVertexBuffers()
{
	if (m_hullPositionBuffer)
	{
		delete m_hullPositionBuffer;
		m_hullPositionBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteHullIndexBuffers()
{
	if (m_hullIndexBuffer)
	{
		delete m_hullIndexBuffer;
		m_hullIndexBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteHullGeometryBuffers()
{
	deleteHullVertexBuffers();
	deleteHullIndexBuffers();
}

// Bounding box buffers
void FlexiChainSingle_SubSceneOverride::updateBoundingBoxGeometryBuffers(const MBoundingBox& bounds)
{
	deleteBoundingBoxVertexBuffers();

	// Acquire vertex buffer resources
	const MVertexBufferDescriptor boundingBoxPositionDesc("", MGeometry::kPosition, MGeometry::kFloat, 3);
	m_boundingBoxPositionBuffer = new MVertexBuffer(boundingBoxPositionDesc);

	// VB for bounding box positions
	if (m_boundingBoxPositionBuffer)
	{
		float* boundingBoxPositions = (float*)m_boundingBoxPositionBuffer->acquire(8, true /*writeOnly*/);
		if (boundingBoxPositions)
		{
			MPoint pMin = bounds.min();
			MPoint pMax = bounds.max();
			
			boundingBoxPositions[0] = (float)pMin.x; boundingBoxPositions[1] = (float)pMin.y; boundingBoxPositions[2] = (float)pMin.z;
			boundingBoxPositions[3] = (float)pMin.x; boundingBoxPositions[4] = (float)pMin.y; boundingBoxPositions[5] = (float)pMax.z;
			boundingBoxPositions[6] = (float)pMax.x; boundingBoxPositions[7] = (float)pMin.y; boundingBoxPositions[8] = (float)pMax.z;
			boundingBoxPositions[9] = (float)pMax.x; boundingBoxPositions[10] = (float)pMin.y; boundingBoxPositions[11] = (float)pMin.z;
			boundingBoxPositions[12] = (float)pMin.x; boundingBoxPositions[13] = (float)pMax.y; boundingBoxPositions[14] = (float)pMin.z;
			boundingBoxPositions[15] = (float)pMin.x; boundingBoxPositions[16] = (float)pMax.y; boundingBoxPositions[17] = (float)pMax.z;
			boundingBoxPositions[18] = (float)pMax.x; boundingBoxPositions[19] = (float)pMax.y; boundingBoxPositions[20] = (float)pMax.z;
			boundingBoxPositions[21] = (float)pMax.x; boundingBoxPositions[22] = (float)pMax.y; boundingBoxPositions[23] = (float)pMin.z;
			
			m_boundingBoxPositionBuffer->commit(boundingBoxPositions);
		}
	}

	// Index buffer is static once created
	if (m_boundingBoxIndexBuffer)
		return;

	m_boundingBoxIndexBuffer = new MHWRender::MIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

	// IB for bounding box item
	if (m_boundingBoxIndexBuffer)
	{
		unsigned int* boundingBoxIndices = (unsigned int*)m_boundingBoxIndexBuffer->acquire(24, true /*writeOnly*/);
		if (boundingBoxIndices)
		{
			boundingBoxIndices[0] = 0; boundingBoxIndices[1] = 1;
			boundingBoxIndices[2] = 1; boundingBoxIndices[3] = 2;
			boundingBoxIndices[4] = 2; boundingBoxIndices[5] = 3;
			boundingBoxIndices[6] = 3; boundingBoxIndices[7] = 0;
			boundingBoxIndices[8] = 4; boundingBoxIndices[9] = 5;
			boundingBoxIndices[10] = 5; boundingBoxIndices[11] = 6;
			boundingBoxIndices[12] = 6; boundingBoxIndices[13] = 7;
			boundingBoxIndices[14] = 7; boundingBoxIndices[15] = 4;
			boundingBoxIndices[16] = 0; boundingBoxIndices[17] = 4;
			boundingBoxIndices[18] = 1; boundingBoxIndices[19] = 5;
			boundingBoxIndices[20] = 2; boundingBoxIndices[21] = 6;
			boundingBoxIndices[22] = 3; boundingBoxIndices[23] = 7;

			m_boundingBoxIndexBuffer->commit(boundingBoxIndices);
		}
	}
}

void FlexiChainSingle_SubSceneOverride::deleteBoundingBoxVertexBuffers()
{
	if (m_boundingBoxPositionBuffer)
	{
		delete m_boundingBoxPositionBuffer;
		m_boundingBoxPositionBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteBoundingBoxIndexBuffers()
{
	if (m_boundingBoxIndexBuffer)
	{
		delete m_boundingBoxIndexBuffer;
		m_boundingBoxIndexBuffer = nullptr;
	}
}

void FlexiChainSingle_SubSceneOverride::deleteBoundingBoxGeometryBuffers()
{
	deleteBoundingBoxVertexBuffers();
	deleteBoundingBoxIndexBuffers();
}

// All buffers
void FlexiChainSingle_SubSceneOverride::deleteVertexBuffers()
{
	deleteRibbonVertexBuffers();
	deleteCurveVertexBuffers();
	deleteNormalsVertexBuffers();
	deleteHullVertexBuffers();
	deleteBoundingBoxVertexBuffers();
}

void FlexiChainSingle_SubSceneOverride::deleteIndexBuffers()
{
	deleteRibbonIndexBuffers();
	deleteCurveIndexBuffers();
	deleteNormalsIndexBuffers();
	deleteHullIndexBuffers();
	deleteBoundingBoxIndexBuffers();
}

void FlexiChainSingle_SubSceneOverride::deleteGeometryBuffers()
{
	deleteVertexBuffers();
	deleteIndexBuffers();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void printShaderParameters(MShaderInstance* shader)
{
	MStringArray paramNames;
	shader->parameterList(paramNames);
	for (unsigned int i = 0; i < paramNames.length(); i++)
		std::cout << paramNames[i] << std::endl;
}

static void printFragmentList()
{
	MRenderer* renderer = MRenderer::theRenderer();
	const MFragmentManager* fragmentManager = renderer->getFragmentManager();
	MStringArray fragmentList;
	fragmentManager->fragmentList(fragmentList);

	for (unsigned int i = 0; i < fragmentList.length(); i++)
		std::cout << fragmentList[i] << std::endl;
}

static void printFragmentXML(const char* fragmentName)
{
	MRenderer* renderer = MRenderer::theRenderer();
	const MFragmentManager* fragmentManager = renderer->getFragmentManager();
	MString buffer;
	fragmentManager->getFragmentXML(fragmentName, buffer);
	std::cout << buffer << std::endl;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------