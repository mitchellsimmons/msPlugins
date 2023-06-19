#pragma once

#include <vector>
#include <algorithm>

#include <maya/MBoundingBox.h>
#include <maya/MDagPath.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnToolContext.h>
#include <maya/MFragmentManager.h>
#include <maya/MFrameContext.h>
#include <maya/MGlobal.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MImage.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxShadingNodeOverride.h>
#include <maya/MPxSubSceneOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MUIDrawManager.h>
#include <maya/MUserData.h>
#include <maya/MVector.h>

#include "flexiChainDouble_locator.h"
#include "utils/color_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class is used to implement viewport 2.0 support for drawing of the FlexiChainDoubleShape
class FlexiChainDouble_SubSceneOverride : public MHWRender::MPxSubSceneOverride
{
public:
	FlexiChainDouble_SubSceneOverride(const MObject& obj);
	~FlexiChainDouble_SubSceneOverride();

	// ------ Const ------
	static const MString kDrawClassification;
	static const MString kDrawRegistrantId;

	// ------ MPxGeometryOverride ------
	MHWRender::DrawAPI supportedDrawAPIs() const override;

	bool requiresUpdate(const MSubSceneContainer& container, const MFrameContext& frameContext) const override;
	void update(MSubSceneContainer& container, const MFrameContext& frameContext) override;

private:
	FlexiChainDouble* m_locator;
	MObject m_locatorObj;

	// ------ Data ------
	// shaders
	MColor m_wireframeColor;
	static const MColor m_surfaceColor;
	static const MColor m_borderDormantColor;
	static const MColor m_curveDormantColor;
	static const MColor m_normalsDormantColor;

	MHWRender::MShaderInstance* m_wireframeShader;
	MHWRender::MShaderInstance* m_surfaceShader;
	MHWRender::MShaderInstance* m_borderDormantShader;
	MHWRender::MShaderInstance* m_curveDormantShader;
	MHWRender::MShaderInstance* m_normalsDormantShader;
	MHWRender::MShaderInstance* m_hullShader;

	// fragment
	const MHWRender::MSamplerState* m_samplerState;
	MHWRender::MTexture *m_rampTexture;
	MHWRender::MTexture *m_diffuseTexture;
	MHWRender::MAttributeParameterMappingList m_mappings;
	MString m_resolvedMapName;
	MString m_resolvedSamplerName;
	static const unsigned int m_texturedPixelCount;
	unsigned char m_texturedPixels[8192]; // Reserve memory for a single row of 2048 pixels (4-channels per pixel)
	unsigned char m_diffusePixel[4];
	
	// render items
	static const float m_ribbonWidthScale;
	static const float m_normalsLengthScale;
	MHWRender::DisplayStatus m_displayStatus;
	MMatrix m_drawTransform;

	static const char* m_surfaceRenderItemName;
	static const char* m_borderActiveRenderItemName;
	static const char* m_borderDormantRenderItemName;
	static const char* m_curveActiveRenderItemName;
	static const char* m_curveDormantRenderItemName;
	static const char* m_normalsActiveRenderItemName;
	static const char* m_normalsDormantRenderItemName;
	static const char* m_hullRenderItemName;
	static const char* m_boundingBoxRenderItemName;

	MHWRender::MRenderItem* m_surfaceRenderItem; // Ownership belongs to MSubSceneContainer passed to update()
	MHWRender::MRenderItem* m_borderActiveRenderItem;
	MHWRender::MRenderItem* m_borderDormantRenderItem;
	MHWRender::MRenderItem* m_curveActiveRenderItem;
	MHWRender::MRenderItem* m_curveDormantRenderItem;
	MHWRender::MRenderItem* m_normalsActiveRenderItem;
	MHWRender::MRenderItem* m_normalsDormantRenderItem;
	MHWRender::MRenderItem* m_hullRenderItem;
	MHWRender::MRenderItem* m_boundingBoxRenderItem;

	// buffer
	MHWRender::MVertexBuffer* m_surfacePositionBuffer;
	MHWRender::MVertexBuffer* m_surfaceNormalBuffer;
	MHWRender::MVertexBuffer* m_surfaceTextureBuffer;
	MHWRender::MVertexBuffer* m_curvePositionBuffer;
	MHWRender::MVertexBuffer* m_normalsPositionBuffer;
	MHWRender::MVertexBuffer* m_hullPositionBuffer;
	MHWRender::MVertexBuffer* m_boundingBoxPositionBuffer;

	MHWRender::MIndexBuffer* m_surfaceIndexBuffer;
	MHWRender::MIndexBuffer* m_borderIndexBuffer;
	MHWRender::MIndexBuffer* m_curveIndexBuffer;
	MHWRender::MIndexBuffer* m_normalsIndexBuffer;
	MHWRender::MIndexBuffer* m_hullIndexBuffer;
	MHWRender::MIndexBuffer* m_boundingBoxIndexBuffer;

	// Cache non-networked plugs
	MPlug m_overrideLevelOfDetailPlug;
	// These properties apply automatically to render items created by Maya (ie. if using MPxSurfaceShape) but not to custom items
	MPlug m_castsShadowsPlug;
	MPlug m_receiveShadowsPlug;

	// ------ States ------
	bool m_isVisible;
	bool m_computeOrientation;
	bool m_ribbonDrawEnabled;
	bool m_curveDrawEnabled;
	bool m_normalsDrawEnabled;
	bool m_hullDrawEnabled;
	bool m_boundingBoxEnabled;
	bool m_castsShadows;
	bool m_receiveShadows;
	bool m_upVectorContextEnabled;
	bool m_scaleAdjustmentContextEnabled;
	bool m_twistAdjustmentContextEnabled;
	int m_surfaceVertexCount;
	int m_curveVertexCount;
	int m_normalsVertexCount;

	// ------ Helpers ------
	void createShaderInstance(const MShaderManager* shaderManager, MFragmentManager* fragmentManager);
	void setDefaultRamp();
	void updateRampTextureUpVectorContext();
	void updateRampTextureScaleAdjustmentContext();
	void updateRampTextureTwistAdjustmentContext();
	void updateSurfaceShader(MShaderInstance* shader, const char* colorParameter, MColor color);
	void updateLineShader(MShaderInstance* shader, const char* lineWidthParameter, float lineWidth, const char* colorParameter, MColor color);

	void updateRibbonGeometryBuffers();
	void deleteRibbonVertexBuffers();
	void deleteRibbonIndexBuffers();
	void deleteRibbonGeometryBuffers();

	void updateCurveGeometryBuffers();
	void deleteCurveVertexBuffers();
	void deleteCurveIndexBuffers();
	void deleteCurveGeometryBuffers();

	void updateNormalsGeometryBuffers();
	void deleteNormalsVertexBuffers();
	void deleteNormalsIndexBuffers();
	void deleteNormalsGeometryBuffers();

	void updateHullGeometryBuffers();
	void deleteHullVertexBuffers();
	void deleteHullIndexBuffers();
	void deleteHullGeometryBuffers();

	void updateBoundingBoxGeometryBuffers(const MBoundingBox& bounds);
	void deleteBoundingBoxVertexBuffers();
	void deleteBoundingBoxIndexBuffers();
	void deleteBoundingBoxGeometryBuffers();

	void deleteVertexBuffers();
	void deleteIndexBuffers();
	void deleteGeometryBuffers();
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void printShaderParameters(MShaderInstance* shader);
static void printFragmentList();
static void printFragmentXML(const char* fragmentName);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------