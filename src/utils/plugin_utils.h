#pragma once

#include <vector>
#include <iostream>

#include <maya/MDrawRegistry.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MPxSubSceneOverride.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>

// MFnPlugin cannot be included in more than one translation unit per project
// Instead, we can forward declare the class, under the condition that the below functions are only ever called from translation units where MFnPlugin has been included (ie. plugin.cpp)
#if MAYA_API_VERSION >= 20180000
#include <maya/MApiNamespace.h>
#else
class MFnPlugin;
#endif

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Validation ------
MStatus validateId(const MTypeId& id, std::vector<int> cache);

// ------ MPxNode ------
template<typename T>
MStatus registerNode(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerNode(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize);

	if (status)
		std::cout << "Registered node : " << T::kTypeName << std::endl;

	return status;
}

template<typename T>
MStatus deregisterNode(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.deregisterNode(T::kTypeId);

	if (status)
		std::cout << "Deregistered node : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxLocatorNode ------
template<typename T>
MStatus registerLocator(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerNode(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize, MPxNode::kLocatorNode, &T::kDrawClassification);

	if (status)
		std::cout << "Registered locator : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxSurfaceShape ------
template<typename T>
MStatus registerShape(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerShape(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize, nullptr, &T::kDrawClassification);

	if (status)
		std::cout << "Registered shape : " << T::kTypeName << std::endl;

	return status;
}

template<typename T, typename TUI>
MStatus registerShape(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerShape(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize, []() -> void* { return new TUI(); }, &T::kDrawClassification);

	if (status)
		std::cout << "Registered shape : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxManipulatorNode ------
template<typename T>
MStatus registerManipulator(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerNode(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize, MPxNode::kManipulatorNode);

	if (status)
		std::cout << "Registered manipulator : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxManipContainer ------
template<typename T>
MStatus registerManipContainer(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerNode(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); }, &T::initialize, MPxNode::kManipContainer);

	if (status)
		std::cout << "Registered manipulator container : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxDrawOverride ------
template<typename TDraw>
MStatus registerDrawOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(TDraw::kDrawClassification, TDraw::kDrawRegistrantId,
		[](const MObject& obj) -> MHWRender::MPxDrawOverride* { return new TDraw(obj); });

	if (status)
		std::cout << "Registered draw override : " << TDraw::kDrawClassification << std::endl;

	return status;
}

template<typename TDraw>
MStatus deregisterDrawOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(TDraw::kDrawClassification, TDraw::kDrawRegistrantId);

	if (status)
		std::cout << "Deregistered draw override : " << TDraw::kDrawClassification << std::endl;

	return status;
}

// ------ MPxGeomtryOverride ------
template<typename TGeometry>
MStatus registerGeometryOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::registerGeometryOverrideCreator(TGeometry::kDrawClassification, TGeometry::kDrawRegistrantId,
		[](const MObject& obj) -> MHWRender::MPxGeometryOverride* { return new TGeometry(obj); });

	if (status)
		std::cout << "Registered geometry override : " << TGeometry::kDrawClassification << std::endl;

	return status;
}

template<typename TGeometry>
MStatus deregisterGeometryOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(TGeometry::kDrawClassification, TGeometry::kDrawRegistrantId);

	if (status)
		std::cout << "Deregistered geometry override : " << TGeometry::kDrawClassification << std::endl;

	return status;
}

// ------ MPxSubSceneOverride ------
template<typename TSubScene>
MStatus registerSubSceneOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::registerSubSceneOverrideCreator(TSubScene::kDrawClassification, TSubScene::kDrawRegistrantId,
		[](const MObject& obj) -> MHWRender::MPxSubSceneOverride* { return new TSubScene(obj); });

	if (status)
		std::cout << "Registered sub-scene override : " << TSubScene::kDrawClassification << std::endl;

	return status;
}

template<typename TSubScene>
MStatus deregisterSubSceneOverride(MFnPlugin& fnPlugin)
{
	MStatus status = MHWRender::MDrawRegistry::deregisterSubSceneOverrideCreator(TSubScene::kDrawClassification, TSubScene::kDrawRegistrantId);

	if (status)
		std::cout << "Deregistered sub-scene override : " << TSubScene::kDrawClassification << std::endl;

	return status;
}

// ------ MPxTransform ------
template<typename TMatrix, typename TTransform>
MStatus registerTransform(MFnPlugin& fnPlugin)
{
	#if MAYA_API_VERSION < 20190000
	MStatus status = fnPlugin.registerTransform(TTransform::kTypeName, TTransform::kTypeId, []() -> void* { return new TTransform(); }, &TTransform::initialize,
		[]() -> void* { return new TMatrix(); }, TMatrix::kTypeId);
	#else
	MStatus status = fnPlugin.registerTransform(TTransform::kTypeName, TTransform::kTypeId, []() -> void* { return new TTransform(); }, &TTransform::initialize,
		[]() -> MPxTransformationMatrix* { return new TMatrix(); }, TMatrix::kTypeId);
	#endif

	if (status)
		std::cout << "Registered transform : " << TTransform::kTypeName << std::endl;

	return status;
}

// ------ MPxData ------
template<typename T>
MStatus registerData(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerData(T::kTypeName, T::kTypeId,
		[]() -> void* { return new T(); });

	if (status)
		std::cout << "Registered data : " << T::kTypeName << std::endl;

	return status;
}

template<typename T>
MStatus deregisterData(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.deregisterData(T::kTypeId);

	if (status)
		std::cout << "Deregistered data : " << T::kTypeName << std::endl;

	return status;
}

// ------ MPxCommand ------
template<typename T>
MStatus registerCommand(MFnPlugin& fnPlugin, bool syntax = true)
{
	MStatus status;
	
	if (syntax)
		fnPlugin.registerCommand(T::kCommandName, []() -> void* { return new T(); }, T::newSyntax);
	else
		fnPlugin.registerCommand(T::kCommandName, []() -> void* { return new T(); });

	if (status)
		std::cout << "Registered command : " << T::kCommandName << std::endl;

	return status;
}

template<typename T>
MStatus deregisterCommand(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.deregisterCommand(T::kCommandName);

	if (status)
		std::cout << "Deregistered command : " << T::kCommandName << std::endl;

	return status;
}

// ------ MPxContextCommand ------
template<typename T>
MStatus registerContextCommand(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.registerContextCommand(T::kCommandName,
		[]() -> void* { return new T(); });
	
	if (status)
		std::cout << "Registered context command : " << T::kCommandName << std::endl;

	return status;
}

template<typename T>
MStatus deregisterContextCommand(MFnPlugin& fnPlugin)
{
	MStatus status = fnPlugin.deregisterContextCommand(T::kCommandName);

	if (status)
		std::cout << "Deregistered context command : " << T::kCommandName << std::endl;

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


// ====== Notes ===============================================================================================================================================================
// Using anonymous lambdas for the creator functions, dynamically allocating memory for new objects of the given template types
// A dynamic allocation is made as Maya needs to be able to receive a generic type (ie. void*) from all of its registered creator function pointers
// It is impossible for Maya to know what our derived MPxNode type is as we are dynamically linking our library during runtime
// ============================================================================================================================================================================