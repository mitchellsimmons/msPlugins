#pragma once

#include <array>
#include <assert.h>
#include <map>
#include <unordered_map>
#include <set>
#include <sstream>
#include <string>

// MetaData.lib
#include <maya/adskDataAssociations.h>
#include <maya/adskDataStream.h>

// OpenMaya.lib
#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MDGModifier.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MObject.h>
#include <maya/MPxCommand.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ P.O.D Hierarchy ------
struct MemberTemplate
{
public:
	const char* name;
	const adsk::Data::Member::eDataType dataType;
	const unsigned int dimension;
	const unsigned int index;
};

template<unsigned int N>
struct StructureTemplate
{
	const char* name;
	const MemberTemplate members[N];
};

template<unsigned int N>
struct ChannelTemplate
{
	const char* name;
	const StructureTemplate<N> structure;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Global Functions ------
// Query
adsk::Data::Stream* getStream(const char* streamName, const char* channelName, adsk::Data::Associations associations);

// Create
template<unsigned int N>
const adsk::Data::Structure* registerStructure(const StructureTemplate<N>& structureTemplate);
adsk::Data::Stream* registerStream(const char* streamName, const char* channelName, adsk::Data::Associations associations, const adsk::Data::Structure* structure);
void registerElement(unsigned int index, adsk::Data::Stream* stream);

// Delete
MStatus deleteStream(const char* streamName, const char* channelName, adsk::Data::Associations associations);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a query/edit/create command that can be used to alter metadata relating to our custom NodeBox QGraphicsItems in the Node Editor
class NodeBoxCommand : public MPxCommand
{
public:
	NodeBoxCommand();
	~NodeBoxCommand() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ Const ------
	static const ChannelTemplate<3> kChannelTemplate;

	static const char* kStreamNameFlag;
	static const char* kStreamNameFlagLong;
	static const char* kFilePathFlag;
	static const char* kFilePathFlagLong;
	static const char* kDeleteFlag;
	static const char* kDeleteFlagLong;
	static const char* kTitleFlag;
	static const char* kTitleFlagLong;
	static const char* kRectFlag;
	static const char* kRectFlagLong;
	static const char* kColorFlag;
	static const char* kColorFlagLong;

	// ------ MPxCommand ------
	MStatus doIt(const MArgList& args) override;

private:
	enum Mode : uint32_t
	{
		kCreate = 1 << 0,
		kEdit = 1 << 1,
		kQuery = 1 << 2
	};

	enum Member : unsigned char
	{
		kTitle = 0,
		kRect = 1,
		kColor = 2
	};

	// ------ Helpers ------
	// Parse
	MStatus parseArgs(const MArgList args);

	// Main
	MStatus doCreate();
	MStatus doQuery();
	MStatus doEdit();

	// Query/Edit
	MStatus queryMember();
	MStatus editMembers();

	// ------ Data ------
	bool m_isDeleteEnabled;
	bool m_isTitleSet;
	bool m_isRectSet;
	bool m_isColorSet;

	Mode m_mode;
	MString m_streamName;

	// Stores parsed data in edit mode or existing metadata in query mode
	std::string m_title;
	std::array<double, 4> m_rect;
	std::array<unsigned char, 4> m_color;

	MObject m_node;
	MDGModifier m_dgMod;
	adsk::Data::Associations m_associations;
	const adsk::Data::Structure* m_structure;
	adsk::Data::Stream* m_stream;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a query/edit/create command that can be used to alter metadata relating to our custom Sticky QGraphicsItems in the Node Editor
class StickyCommand : public MPxCommand
{
public:
	StickyCommand();
	~StickyCommand() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ Const ------
	static const ChannelTemplate<4> kChannelTemplate;

	static const char* kStreamNameFlag;
	static const char* kStreamNameFlagLong;
	static const char* kDeleteFlag;
	static const char* kDeleteFlagLong;
	static const char* kTitleFlag;
	static const char* kTitleFlagLong;
	static const char* kRectFlag;
	static const char* kRectFlagLong;
	static const char* kColorFlag;
	static const char* kColorFlagLong;
	static const char* kTextFlag;
	static const char* kTextFlagLong;

	// ------ MPxCommand ------
	MStatus doIt(const MArgList& args) override;

private:
	enum Mode : uint32_t
	{
		kCreate = 1 << 0,
		kEdit = 1 << 1,
		kQuery = 1 << 2
	};

	enum Member : unsigned char
	{
		kTitle = 0,
		kRect = 1,
		kColor = 2,
		kText = 3
	};

	// ------ Helpers ------
	// Parse
	MStatus parseArgs(const MArgList args);

	// Main
	MStatus doCreate();
	MStatus doQuery();
	MStatus doEdit();

	// Query/Edit
	MStatus queryMember();
	MStatus editMembers();

	// ------ Data ------
	bool m_isDeleteEnabled;
	bool m_isTitleSet;
	bool m_isRectSet;
	bool m_isColorSet;
	bool m_isTextSet;

	Mode m_mode;
	MString m_streamName;

	// Stores parsed data in edit mode or existing metadata in query mode
	std::string m_title;
	std::array<double, 4> m_rect;
	std::array<unsigned char, 4> m_color;
	std::string m_text;

	MObject m_node;
	MDGModifier m_dgMod;
	adsk::Data::Associations m_associations;
	const adsk::Data::Structure* m_structure;
	adsk::Data::Stream* m_stream;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------