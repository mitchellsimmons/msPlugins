#include "nodeEditorTabInfoMetadata_cmd.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
	NOTE
	----
	Commands contained within this file replace those found in the "nodeEditorSceneMetadata_cmd" file
	- We have chosen to apply our metadata to the default "MayaNodeEditorSavedTabsInfo" node which manages the internal Node Editor layout data
	- This way our custom data is saved directly with Maya's internal data
	- We are simply extending the default behaviour Maya has already implemented
	- This way our custom data inherits all of the default behaviour Maya implements to manage Node Editor data
		- Eg. Node Editor tab data is not imported between files as the "MayaNodeEditorSavedTabsInfo" node is not saved with imports
		- Our custom data automatically inherits this same functionality
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Explicit Instantiations

template StructureTemplate<3>;
template StructureTemplate<4>;
template ChannelTemplate<3>;
template ChannelTemplate<4>;
template const adsk::Data::Structure* registerStructure(const StructureTemplate<3>& structureTemplate);
template const adsk::Data::Structure* registerStructure(const StructureTemplate<4>& structureTemplate);

// ------ Defines ------

#define kErrorBothModeFlagsSet \
	"Cannot specify \"-edit\" and \"-query\" flags simultaneously."

#define kErrorFlagNotSet \
	"The \"^1s\" flag is required by this command."

#define kErrorParsingFlag \
	"Error parsing flag \"^1s\"."

#define kErrorNoTabInfoNode \
	"The \"MayaNodeEditorSavedTabsInfo\" node does not exist."

#define kErrorRetrievingTabInfoNode \
	"Error retrieving the \"MayaNodeEditorSavedTabsInfo\" node."

#define kErrorEditModeRequired \
	"The \"-edit\" flag must be used with the \"^1s\" flag."

#define kWarningIgnoreMemberFlags \
	"One or more member flags were used in combination with the \"^1s\" flag, member flags will be ignored."

#define kErrorDefaultsRequired \
	"Default values must be provided for all member flags when in create mode."

#define kErrorMemberFlagNotSet \
	"At least one member flag must be specified when the \"-edit\" or \"-query\" flag is used."

#define kErrorMultipleMemberFlagsOnQuery \
	"Cannot specify more than one member flag when the \"-query\" flag is used."

#define kErrorSetHandleData \
	"Error setting data for \"^1s\" member of \"^2s\" structure (errors = ^3s)."

#define kErrorExistingStream \
	"The stream \"^1s\" already exists in the \"^2s\" channel, stream registration failed."

#define kErrorCreateStream \
	"Error creating stream \"^1s\" for the \"^2s\" channel, stream registration failed."

#define kErrorRetrieveStream \
	"The stream \"^1s\" was not found in the \"^2s\" channel, stream lookup failed."

#define kErrorRemovingStream \
	"The stream \"^1s\" was not found in the \"^2s\" channel, stream removal failed."

// ------ Global Functions ------

adsk::Data::Stream* getStream(const char* streamName, const char* channelName, adsk::Data::Associations associations)
{
	std::string stdChannelName{ channelName };
	std::string stdStreamName{ streamName };

	// Retrieve or create the Channel
	adsk::Data::Channel channel = associations.channel(stdChannelName);

	// Retrieve the Stream
	adsk::Data::Stream* stream = channel.findDataStream(stdStreamName);
	if (!stream)
	{
		MString msg;
		MString msgFormat = kErrorRetrieveStream;
		msg.format(msgFormat, streamName, channelName);
		MGlobal::displayError(msg);
		return nullptr;
	}

	return stream;
}

/*	Description
	-----------
	Registers a custom Structure with the global list of metadata Structures which can then be used to generate Streams
	The new Structure will be modelled from the given Structure data, providing a description of the data required to serialize an entity with the scene    */
template<unsigned int N>
const adsk::Data::Structure* registerStructure(const StructureTemplate<N>& structureTemplate)
{
	// Check if our Structure is registered
	adsk::Data::Structure* structure = nullptr;
	for (auto it = adsk::Data::Structure::allStructures().begin(); it != adsk::Data::Structure::allStructures().end(); ++it)
	{
		if (std::string((*it)->name()) == std::string(structureTemplate.name))
		{
			structure = *it;
			break;
		}
	}

	// Register our Structure since it is not registered yet
	if (structure == nullptr)
	{
		structure = adsk::Data::Structure::create();
		structure->setName(structureTemplate.name);
		unsigned char index = 0;
		for (const MemberTemplate& member : structureTemplate.members)
		{
			// Enforce consecutive indexing of members
			assert(index++ == member.index);
			structure->addMember(member.dataType, member.dimension, member.name);
		}

		adsk::Data::Structure::registerStructure(*structure);
	}

	return structure;
}

adsk::Data::Stream* registerStream(const char* streamName, const char* channelName, adsk::Data::Associations associations, const adsk::Data::Structure* structure)
{
	std::string stdChannelName{ channelName };
	std::string stdStreamName{ streamName };

	// Retrieve or create the Channel
	adsk::Data::Channel channel = associations.channel(stdChannelName);

	// Check the Stream does not already exist since Channels allow duplicates
	if (channel.findDataStream(stdStreamName))
	{
		MString msg;
		MString msgFormat = kErrorExistingStream;
		msg.format(msgFormat, streamName, channelName);
		MGlobal::displayError(msg);
		return nullptr;
	}

	// Create the Stream and register it with the Channel
	const adsk::Data::Stream cstream{ *structure, stdStreamName };
	adsk::Data::Stream* stream = channel.setDataStream(cstream);
	if (!stream)
	{
		MString msg;
		MString msgFormat = kErrorCreateStream;
		msg.format(msgFormat, streamName, channelName);
		MGlobal::displayError(msg);
		return nullptr;
	}

	return stream;
}

/*	Description
	-----------
	Constructs a new Handle from the Structure which is currently being used by the given Stream
	Adds a new Element to the given Stream using the Handle (initialized with default values)
	A reference to the Element can be retrieved by calling Stream::element(index)    */
void registerElement(unsigned int index, adsk::Data::Stream* stream)
{
	adsk::Data::Handle handle{ stream->structure() };
	stream->setElement(index, handle);
}

MStatus deleteStream(const char* streamName, const char* channelName, adsk::Data::Associations associations)
{
	std::string stdChannelName{ channelName };
	std::string stdStreamName{ streamName };

	// Retrieve or create the Channel
	adsk::Data::Channel channel = associations.channel(stdChannelName);

	// Check the Stream does not already exist since Channels allow duplicates
	if (!channel.removeDataStream(stdStreamName))
	{
		MString msg;
		MString msgFormat = kErrorRemovingStream;
		msg.format(msgFormat, streamName, channelName);
		MGlobal::displayError(msg);
		return MStatus::kFailure;
	}

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The mrs_NodeBox command writes metadata to the current nodeGraphEditorInfo node, allowing NodeBox QGraphicsItems to persist between sessions in the Node Editor
	The current scene must contain a nodeGraphEditorInfo node named "MayaNodeEditorSavedTabsInfo" otherwise this command will fail
	Specifically, metadata produced by this command is stored in the mrs_NodeBox_channel which is a descendant Channel of the nodeGraphEditorInfo node's Associations
	The Channel stores Streams which are templated from the mrs_NodeBox_structure, each Stream stores the data for a single QGraphicsItem (ie. one Element per Stream)
	In create mode this command enforces the proper initialization of a Stream by requiring all Member flags are specified (see flag documentation below)
	This command also supports both query and edit modes such that it is possible to edit/query Members of an existing Stream
	In edit mode, at least one Member flag must be specified and in query mode exactly one Member flag must be specified

	MEL Command
	-----------
	mrs_NodeBox [-streamName string] [-delete boolean] [-title string] [-rect floatArray] [-color intArray]

	Examples
	--------
	Create Mode :
		mrs_NodeBox -streamName "streamName" -title "NodeBox" -rect 0 0 500 500 -color 255 255 255 255
	Edit Mode :
		mrs_NodeBox -e -streamName "streamName" -title "My New Title"
		mrs_NodeBox -e -delete -streamName "streamName"
	Query Mode :
		mrs_NodeBox -q -streamName "streamName" -title

	Flags
	-----
	-streamName(-sn)
		In create mode, this flag specifies the name of the stream to create
		In query and edit modes, this flag specifies the name of an existing Stream from which to retrieve or update Member data
		This flag is required in all modes

	-delete (-d)
		This flag specifies that the given Stream should be deleted
		This flag is only valid in edit mode

	-title(-t)
		Member flag
		This flag is used to operate on the "title" Member of the specified Stream and must be provided in create mode

	-rect(-r)
		Member flag
		This flag is used to operate on the "rect" Member of the specified Stream and must be provided in create mode

	-color(-c)
		Member flag
		This flag is used to operate on the "color" Member of the specified Stream and must be provided in create mode
*/

NodeBoxCommand::NodeBoxCommand() :
	m_isDeleteEnabled{ false },
	m_isTitleSet{ false },
	m_isRectSet{ false },
	m_isColorSet{ false },
	m_structure{ nullptr },
	m_stream{ nullptr }{}

NodeBoxCommand::~NodeBoxCommand()
{
	m_structure = nullptr;
	m_stream = nullptr;
}

// ------ Const ------
// Channel
const ChannelTemplate<3> NodeBoxCommand::kChannelTemplate
{
	"mrs_nodeEditor_NodeBox_channel",
	StructureTemplate<3>
	{
		"mrs_nodeEditor_NodeBox_structure",
		{
			{"title", adsk::Data::Member::kString, 1, kTitle},
			{"rect", adsk::Data::Member::kDouble, 4, kRect},
			{"color", adsk::Data::Member::kUInt8, 4, kColor}
		}
	}
};

// Flags
const char* NodeBoxCommand::kStreamNameFlag = "-sn";
const char* NodeBoxCommand::kStreamNameFlagLong = "-streamName";
const char* NodeBoxCommand::kDeleteFlag = "-d";
const char* NodeBoxCommand::kDeleteFlagLong = "-delete";
const char* NodeBoxCommand::kTitleFlag = "-t";
const char* NodeBoxCommand::kTitleFlagLong = "-title";
const char* NodeBoxCommand::kRectFlag = "-r";
const char* NodeBoxCommand::kRectFlagLong = "-rect";
const char* NodeBoxCommand::kColorFlag = "-c";
const char* NodeBoxCommand::kColorFlagLong = "-color";

// ------ Registration ------
MSyntax NodeBoxCommand::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(kStreamNameFlag, kStreamNameFlagLong, MSyntax::kString);
	syntax.addFlag(kDeleteFlag, kDeleteFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kTitleFlag, kTitleFlagLong, MSyntax::kString);
	syntax.addFlag(kRectFlag, kRectFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kColorFlag, kColorFlagLong, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned);

	syntax.makeFlagQueryWithFullArgs(kStreamNameFlag, false);

	syntax.enableQuery(true);
	syntax.enableEdit(true);

	return syntax;
}

// --- Helpers ---
MStatus NodeBoxCommand::parseArgs(const MArgList args)
{
	MArgParser argParser(syntax(), args);

	// Validate and parse the mode
	if (argParser.isEdit() && argParser.isQuery())
	{
		displayError(kErrorBothModeFlagsSet);
		return MStatus::kFailure;
	}
	else if (argParser.isQuery() || argParser.isEdit())
		m_mode = argParser.isQuery() ? kQuery : kEdit;
	else
		m_mode = kCreate;

	// Ensure the nodeGraphEditorInfo node exists
	MSelectionList sel;
	if (!sel.add("MayaNodeEditorSavedTabsInfo"))
	{
		displayError(kErrorNoTabInfoNode);
		return MStatus::kFailure;
	}
	if (!sel.getDependNode(0, m_node))
	{
		displayError(kErrorRetrievingTabInfoNode);
		return MStatus::kFailure;
	}

	// Validate and parse -streamName
	if (!argParser.isFlagSet(kStreamNameFlagLong))
	{
		MString msg;
		MString msgFormat = kErrorFlagNotSet;
		msg.format(msgFormat, kStreamNameFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	if (!argParser.getFlagArgument(kStreamNameFlagLong, 0, m_streamName))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kStreamNameFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}

	m_isDeleteEnabled = argParser.isFlagSet(kDeleteFlagLong); // Temp value
	m_isTitleSet = argParser.isFlagSet(kTitleFlagLong);
	m_isRectSet = argParser.isFlagSet(kRectFlagLong);
	m_isColorSet = argParser.isFlagSet(kColorFlagLong);
	char numMemberFlags = m_isTitleSet + m_isRectSet + m_isColorSet;

	// Validate and parse the -delete flag
	if (m_isDeleteEnabled)
	{
		if (m_mode != kEdit)
		{
			MString msg;
			MString msgFormat = kErrorEditModeRequired;
			msg.format(msgFormat, kDeleteFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
		if (!argParser.getFlagArgument(kDeleteFlagLong, 0, m_isDeleteEnabled))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kDeleteFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
		if (m_isDeleteEnabled && numMemberFlags != 0)
		{
			MString msg;
			MString msgFormat = kWarningIgnoreMemberFlags;
			msg.format(msgFormat, kDeleteFlagLong);
			displayWarning(msg);
		}
	}

	// Validate Member flags against the mode
	if (m_mode == kCreate)
	{
		if (numMemberFlags != 3)
		{
			displayError(kErrorDefaultsRequired);
			return MStatus::kFailure;
		}
	}
	else if(m_mode == kEdit)
	{
		if (numMemberFlags == 0 && !m_isDeleteEnabled)
		{
			displayError(kErrorMemberFlagNotSet);
			return MStatus::kFailure;
		}
	}
	else if (m_mode == kQuery)
	{
		if (numMemberFlags == 0)
		{
			displayError(kErrorMemberFlagNotSet);
			return MStatus::kFailure;
		}
		else if (numMemberFlags > 1)
		{
			displayError(kErrorMultipleMemberFlagsOnQuery);
			return MStatus::kFailure;
		}
	}

	// Parse Member flags
	if (m_mode == kEdit || m_mode == kCreate)
	{
		if (m_isTitleSet)
		{
			MString title;
			if (!argParser.getFlagArgument(kTitleFlagLong, 0, title))
			{
				MString msg;
				MString msgFormat = kErrorParsingFlag;
				msg.format(msgFormat, kTitleFlagLong);
				displayError(msg);
				return MStatus::kFailure;
			}
			m_title = title.asChar();
		}
		if (m_isRectSet)
		{
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (!argParser.getFlagArgument(kRectFlagLong, i, m_rect[i]))
				{
					MString msg;
					MString msgFormat = kErrorParsingFlag;
					msg.format(msgFormat, kRectFlagLong);
					displayError(msg);
					return MStatus::kFailure;
				}
			}
		}
		if (m_isColorSet)
		{
			for (unsigned int i = 0; i < 4; ++i)
			{
				unsigned int channel;
				if (!argParser.getFlagArgument(kColorFlagLong, i, channel))
				{
					MString msg;
					MString msgFormat = kErrorParsingFlag;
					msg.format(msgFormat, kColorFlagLong);
					displayError(msg);
					return MStatus::kFailure;
				}
				m_color[i] = (unsigned char)channel;
			}
		}
	}

	return MStatus::kSuccess;
}

MStatus NodeBoxCommand::doCreate()
{
	MStatus status;
	
	m_structure = registerStructure<3>(kChannelTemplate.structure);
	m_stream = registerStream(m_streamName.asChar(), kChannelTemplate.name, m_associations, m_structure);
	if (m_stream == nullptr)
		return MStatus::kFailure;

	registerElement(0, m_stream);
	if (!status)
		return status;

	status = editMembers();
	if (!status)
		return status;

	m_dgMod.setMetadata(m_node, m_associations);
	status = m_dgMod.doIt();

	return status;
}

MStatus NodeBoxCommand::doQuery()
{
	MStatus status;

	m_stream = getStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	if (m_stream == nullptr)
		return MStatus::kFailure;
	m_structure = &m_stream->structure();

	status = queryMember();

	return status;
}

MStatus NodeBoxCommand::doEdit()
{
	MStatus status;

	m_stream = getStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	if (m_stream == nullptr)
		return MStatus::kFailure;
	m_structure = &m_stream->structure();

	if (m_isDeleteEnabled)
		status = deleteStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	else
		status = editMembers();

	if (!status)
		return status;

	m_dgMod.setMetadata(m_node, m_associations);
	status = m_dgMod.doIt();

	return status;
}

MStatus NodeBoxCommand::editMembers()
{
	std::string errors;

	// The stream is expected to have a single element at index 0, get a reference to the internal data via a Handle
	adsk::Data::Handle handle = m_stream->element(0);

	// Iterate over Members of the Structure then set the value of any parsed member flags into the Handle
	// This will update the stream immediately as it is a direct reference (ie. no need to call Stream::setElement with the Handle)
	for (auto structIt = m_structure->begin(); structIt != m_structure->end(); ++structIt)
	{
		// Set any parsed data using the Member index (not guaranteed to be sequential whilst iterating)
		if (structIt.index() == kTitle && m_isTitleSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);
			
			// Handle::asChar returns a pointer to the first char* of the current metadata (not suitable since the string length will likely change)
			if (handle.fromStr(m_title, 0, errors) != 0)
			{
				MString msg;
				MString msgFormat = kErrorSetHandleData;
				msg.format(msgFormat, kChannelTemplate.structure.members[kTitle].name, kChannelTemplate.structure.name, errors.c_str());
				displayError(msg);
				return MStatus::kFailure;
			}
		}
		else if (structIt.index() == kRect && m_isRectSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kDouble);
			double* metadata = handle.asDouble();
			std::memcpy(metadata, m_rect.data(), sizeof m_rect);
		}
		else if (structIt.index() == kColor && m_isColorSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kUInt8);
			unsigned char* metadata = handle.asUInt8();
			std::memcpy(metadata, m_color.data(), sizeof m_color);
		}
	}

	return MStatus::kSuccess;
}

MStatus NodeBoxCommand::queryMember()
{
	// The stream is expected to have a single element at index 0, get a reference to the internal data via a Handle
	adsk::Data::Handle handle = m_stream->element(0);

	// Iterate over Members of the Structure and check if the current interation matched the set member flag
	for (auto structIt = m_structure->begin(); structIt != m_structure->end(); ++structIt)
	{
		if (structIt.index() == kTitle && m_isTitleSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);
			char** metadata = handle.asString();
			m_title.assign(*metadata);
		}
		else if (structIt.index() == kRect && m_isRectSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kDouble);
			double* metadata = handle.asDouble();
			std::memcpy(m_rect.data(), metadata, sizeof m_rect);
		}
		else if (structIt.index() == kColor && m_isColorSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kUInt8);
			unsigned char* metadata = handle.asUInt8();
			std::memcpy(m_color.data(), metadata, sizeof m_color);
		}
	}

	return MStatus::kSuccess;
}

// ------ MPxCommand ------
MStatus NodeBoxCommand::doIt(const MArgList& args)
{
	MStatus status = parseArgs(args);
	if (!status)
		return status;

	MFnDependencyNode fnNode{ m_node };
	m_associations = fnNode.metadata();

	if (m_mode == kCreate)
	{
		status = doCreate();
		setResult((bool)status);
	}
	else if (m_mode == kEdit)
	{
		status = doEdit();
		setResult((bool)status);
	}
	else if (m_mode == kQuery)
	{
		status = doQuery();
		if (status)
		{
			if (m_isTitleSet)
				setResult(m_title.c_str());
			else if (m_isRectSet)
			{
				MDoubleArray res;
				res.setLength((unsigned)m_rect.size());
				for (unsigned int i = 0; i < m_rect.size(); ++i)
					res[i] = m_rect[i];
				setResult(res);
			}
			else if (m_isColorSet)
			{
				MIntArray res;
				res.setLength((unsigned)m_color.size());
				for (unsigned int i = 0; i < m_color.size(); ++i)
					res[i] = m_color[i];
				setResult(res);
			}
		}
		else
			setResult(false);
	}

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The mrs_Sticky command writes metadata to the current nodeGraphEditorInfo node, allowing Sticky QGraphicsItems to persist between sessions in the Node Editor
	The current scene must contain a nodeGraphEditorInfo node named "MayaNodeEditorSavedTabsInfo" otherwise this command will fail
	Specifically, metadata produced by this command is stored in the mrs_Sticky_channel which is a descendant Channel of the nodeGraphEditorInfo node's Associations
	The Channel stores Streams which are templated from the mrs_Sticky_structure, each Stream stores the data for a single QGraphicsItem (ie. one Element per Stream)
	In create mode this command enforces the proper initialization of a Stream by requiring all Member flags are specified (see flag documentation below)
	This command also supports both query and edit modes such that it is possible to edit/query Members of an existing Stream
	In edit mode, at least one Member flag must be specified and in query mode exactly one Member flag must be specified

	MEL Command
	-----------
	mrs_Sticky [-streamName string] [-delete boolean] [-title string] [-rect floatArray] [-color intArray] [-text string]

	Examples
	--------
	Create Mode :
		mrs_Sticky -streamName "streamName" -title "Sticky" -rect 0 0 500 500 -color 255 255 255 255 -text "This is a comment"
	Edit Mode :
		mrs_Sticky -e -streamName "streamName" -title "My New Title"
		mrs_Sticky -e -delete -streamName "streamName"
	Query Mode :
		mrs_Sticky -q -streamName "streamName" -title

	Flags
	-----
	-streamName(-sn)
		In create mode, this flag specifies the name of the stream to create
		In query and edit modes, this flag specifies the name of an existing Stream from which to retrieve or update Member data
		This flag is required in all modes

	-delete (-d)
		This flag specifies that the given Stream should be deleted
		This flag is only valid in edit mode

	-title(-t)
		Member flag
		This flag is used to operate on the "title" Member of the specified Stream and must be provided in create mode

	-rect(-r)
		Member flag
		This flag is used to operate on the "rect" Member of the specified Stream and must be provided in create mode

	-color(-c)
		Member flag
		This flag is used to operate on the "color" Member of the specified Stream and must be provided in create mode

	-text(-txt)
		Member flag
		This flag is used to operate on the "text" Member of the specified Stream and must be provided in create mode
*/

StickyCommand::StickyCommand() :
	m_isDeleteEnabled{ false },
	m_isTitleSet{ false },
	m_isRectSet{ false },
	m_isColorSet{ false },
	m_isTextSet{ false },
	m_structure{ nullptr },
	m_stream{ nullptr }{}

StickyCommand::~StickyCommand()
{
	m_structure = nullptr;
	m_stream = nullptr;
}

// ------ Const ------
// Channel
const ChannelTemplate<4> StickyCommand::kChannelTemplate
{
	"mrs_nodeEditor_Sticky_channel",
	StructureTemplate<4>
	{
		"mrs_nodeEditor_Sticky_structure",
		{
			{"title", adsk::Data::Member::kString, 1, kTitle},
			{"rect", adsk::Data::Member::kDouble, 4, kRect},
			{"color", adsk::Data::Member::kUInt8, 4, kColor},
			{"text", adsk::Data::Member::kString, 1, kText}
		}
	}
};

// Flags
const char* StickyCommand::kStreamNameFlag = "-sn";
const char* StickyCommand::kStreamNameFlagLong = "-streamName";
const char* StickyCommand::kDeleteFlag = "-d";
const char* StickyCommand::kDeleteFlagLong = "-delete";
const char* StickyCommand::kTitleFlag = "-t";
const char* StickyCommand::kTitleFlagLong = "-title";
const char* StickyCommand::kRectFlag = "-r";
const char* StickyCommand::kRectFlagLong = "-rect";
const char* StickyCommand::kColorFlag = "-c";
const char* StickyCommand::kColorFlagLong = "-color";
const char* StickyCommand::kTextFlag = "-txt";
const char* StickyCommand::kTextFlagLong = "-text";

// ------ Registration ------
MSyntax StickyCommand::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag(kStreamNameFlag, kStreamNameFlagLong, MSyntax::kString);
	syntax.addFlag(kDeleteFlag, kDeleteFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kTitleFlag, kTitleFlagLong, MSyntax::kString);
	syntax.addFlag(kRectFlag, kRectFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kColorFlag, kColorFlagLong, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned);
	syntax.addFlag(kTextFlag, kTextFlagLong, MSyntax::kString);

	syntax.makeFlagQueryWithFullArgs(kStreamNameFlag, false);

	syntax.enableQuery(true);
	syntax.enableEdit(true);

	return syntax;
}

// --- Helpers ---
MStatus StickyCommand::parseArgs(const MArgList args)
{
	MArgParser argParser(syntax(), args);

	// Validate and parse the mode
	if (argParser.isEdit() && argParser.isQuery())
	{
		displayError(kErrorBothModeFlagsSet);
		return MStatus::kFailure;
	}
	else if (argParser.isQuery() || argParser.isEdit())
		m_mode = argParser.isQuery() ? kQuery : kEdit;
	else
		m_mode = kCreate;

	// Ensure the nodeGraphEditorInfo node exists
	MSelectionList sel;
	if (!sel.add("MayaNodeEditorSavedTabsInfo"))
	{
		displayError(kErrorNoTabInfoNode);
		return MStatus::kFailure;
	}
	if (!sel.getDependNode(0, m_node))
	{
		displayError(kErrorRetrievingTabInfoNode);
		return MStatus::kFailure;
	}

	// Validate and parse -streamName
	if (!argParser.isFlagSet(kStreamNameFlagLong))
	{
		MString msg;
		MString msgFormat = kErrorFlagNotSet;
		msg.format(msgFormat, kStreamNameFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	if (!argParser.getFlagArgument(kStreamNameFlagLong, 0, m_streamName))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kStreamNameFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}

	m_isDeleteEnabled = argParser.isFlagSet(kDeleteFlagLong); // Temp value
	m_isTitleSet = argParser.isFlagSet(kTitleFlagLong);
	m_isRectSet = argParser.isFlagSet(kRectFlagLong);
	m_isColorSet = argParser.isFlagSet(kColorFlagLong);
	m_isTextSet = argParser.isFlagSet(kTextFlagLong);
	char numMemberFlags = m_isTitleSet + m_isRectSet + m_isColorSet + m_isTextSet;

	// Validate and parse the -delete flag
	if (m_isDeleteEnabled)
	{
		if (m_mode != kEdit)
		{
			MString msg;
			MString msgFormat = kErrorEditModeRequired;
			msg.format(msgFormat, kDeleteFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
		if (!argParser.getFlagArgument(kDeleteFlagLong, 0, m_isDeleteEnabled))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kDeleteFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
		if (m_isDeleteEnabled && numMemberFlags != 0)
		{
			MString msg;
			MString msgFormat = kWarningIgnoreMemberFlags;
			msg.format(msgFormat, kDeleteFlagLong);
			displayWarning(msg);
		}
	}

	// Validate Member flags against the mode
	if (m_mode == kCreate)
	{
		if (numMemberFlags != 4)
		{
			displayError(kErrorDefaultsRequired);
			return MStatus::kFailure;
		}
	}
	else if(m_mode == kEdit)
	{
		if (numMemberFlags == 0 && !m_isDeleteEnabled)
		{
			displayError(kErrorMemberFlagNotSet);
			return MStatus::kFailure;
		}
	}
	else if (m_mode == kQuery)
	{
		if (numMemberFlags == 0)
		{
			displayError(kErrorMemberFlagNotSet);
			return MStatus::kFailure;
		}
		else if (numMemberFlags > 1)
		{
			displayError(kErrorMultipleMemberFlagsOnQuery);
			return MStatus::kFailure;
		}
	}

	// Parse Member flags
	if (m_mode == kEdit || m_mode == kCreate)
	{
		if (m_isTitleSet)
		{
			MString title;
			if (!argParser.getFlagArgument(kTitleFlagLong, 0, title))
			{
				MString msg;
				MString msgFormat = kErrorParsingFlag;
				msg.format(msgFormat, kTitleFlagLong);
				displayError(msg);
				return MStatus::kFailure;
			}
			m_title = title.asChar();
		}
		if (m_isRectSet)
		{
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (!argParser.getFlagArgument(kRectFlagLong, i, m_rect[i]))
				{
					MString msg;
					MString msgFormat = kErrorParsingFlag;
					msg.format(msgFormat, kRectFlagLong);
					displayError(msg);
					return MStatus::kFailure;
				}
			}
		}
		if (m_isColorSet)
		{
			for (unsigned int i = 0; i < 4; ++i)
			{
				unsigned int channel;
				if (!argParser.getFlagArgument(kColorFlagLong, i, channel))
				{
					MString msg;
					MString msgFormat = kErrorParsingFlag;
					msg.format(msgFormat, kColorFlagLong);
					displayError(msg);
					return MStatus::kFailure;
				}
				m_color[i] = (unsigned char)channel;
			}
		}
		if (m_isTextSet)
		{
			MString text;
			if (!argParser.getFlagArgument(kTextFlagLong, 0, text))
			{
				MString msg;
				MString msgFormat = kErrorParsingFlag;
				msg.format(msgFormat, kTextFlagLong);
				displayError(msg);
				return MStatus::kFailure;
			}
			m_text = text.asChar();
		}
	}

	return MStatus::kSuccess;
}

MStatus StickyCommand::doCreate()
{
	MStatus status;
	
	m_structure = registerStructure<4>(kChannelTemplate.structure);
	m_stream = registerStream(m_streamName.asChar(), kChannelTemplate.name, m_associations, m_structure);
	if (m_stream == nullptr)
		return MStatus::kFailure;

	registerElement(0, m_stream);
	if (!status)
		return status;

	status = editMembers();
	if (!status)
		return status;

	m_dgMod.setMetadata(m_node, m_associations);
	status = m_dgMod.doIt();

	return status;
}

MStatus StickyCommand::doQuery()
{
	MStatus status;

	m_stream = getStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	if (m_stream == nullptr)
		return MStatus::kFailure;
	m_structure = &m_stream->structure();

	status = queryMember();

	return status;
}

MStatus StickyCommand::doEdit()
{
	MStatus status;

	m_stream = getStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	if (m_stream == nullptr)
		return MStatus::kFailure;
	m_structure = &m_stream->structure();

	if (m_isDeleteEnabled)
		status = deleteStream(m_streamName.asChar(), kChannelTemplate.name, m_associations);
	else
		status = editMembers();

	if (!status)
		return status;

	m_dgMod.setMetadata(m_node, m_associations);
	status = m_dgMod.doIt();

	return status;
}

MStatus StickyCommand::editMembers()
{
	std::string errors;

	// The stream is expected to have a single element at index 0, get a reference to the internal data via a Handle
	adsk::Data::Handle handle = m_stream->element(0);

	// Iterate over Members of the Structure then set the value of any parsed member flags into the Handle
	// This will update the stream immediately as it is a direct reference (ie. no need to call Stream::setElement with the Handle)
	for (auto structIt = m_structure->begin(); structIt != m_structure->end(); ++structIt)
	{
		// Set any parsed data using the Member index (not guaranteed to be sequential whilst iterating)
		if (structIt.index() == kTitle && m_isTitleSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);
			
			// Handle::asChar returns a pointer to the first char* of the current metadata (not suitable since the string length will likely change)
			if (handle.fromStr(m_title, 0, errors) != 0)
			{
				MString msg;
				MString msgFormat = kErrorSetHandleData;
				msg.format(msgFormat, kChannelTemplate.structure.members[kTitle].name, kChannelTemplate.structure.name, errors.c_str());
				displayError(msg);
				return MStatus::kFailure;
			}
		}
		else if (structIt.index() == kRect && m_isRectSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kDouble);
			double* metadata = handle.asDouble();
			std::memcpy(metadata, m_rect.data(), sizeof m_rect);
		}
		else if (structIt.index() == kColor && m_isColorSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kUInt8);
			unsigned char* metadata = handle.asUInt8();
			std::memcpy(metadata, m_color.data(), sizeof m_color);
		}
		else if (structIt.index() == kText && m_isTextSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);

			if (handle.fromStr(m_text, 0, errors) != 0)
			{
				MString msg;
				MString msgFormat = kErrorSetHandleData;
				msg.format(msgFormat, kChannelTemplate.structure.members[kText].name, kChannelTemplate.structure.name, errors.c_str());
				displayError(msg);
				return MStatus::kFailure;
			}
		}
	}

	return MStatus::kSuccess;
}

MStatus StickyCommand::queryMember()
{
	// The stream is expected to have a single element at index 0, get a reference to the internal data via a Handle
	adsk::Data::Handle handle = m_stream->element(0);

	// Iterate over Members of the Structure and check if the current interation matched the set member flag
	for (auto structIt = m_structure->begin(); structIt != m_structure->end(); ++structIt)
	{
		if (structIt.index() == kTitle && m_isTitleSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);
			char** metadata = handle.asString();
			m_title.assign(*metadata);
		}
		else if (structIt.index() == kRect && m_isRectSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kDouble);
			double* metadata = handle.asDouble();
			std::memcpy(m_rect.data(), metadata, sizeof m_rect);
		}
		else if (structIt.index() == kColor && m_isColorSet)
		{
			assert(structIt->length() == 4);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kUInt8);
			unsigned char* metadata = handle.asUInt8();
			std::memcpy(m_color.data(), metadata, sizeof m_color);
		}
		else if (structIt.index() == kText && m_isTextSet)
		{
			assert(structIt->length() == 1);
			handle.setPositionByMemberIndex(structIt.index());
			assert(handle.hasData());
			assert(handle.dataType() == adsk::Data::Member::kString);
			char** metadata = handle.asString();
			m_text.assign(*metadata);
		}
	}

	return MStatus::kSuccess;
}

// ------ MPxCommand ------
MStatus StickyCommand::doIt(const MArgList& args)
{
	MStatus status = parseArgs(args);
	if (!status)
		return status;

	MFnDependencyNode fnNode{ m_node };
	m_associations = fnNode.metadata();

	if (m_mode == kCreate)
	{
		status = doCreate();
		setResult((bool)status);
	}
	else if (m_mode == kEdit)
	{
		status = doEdit();
		setResult((bool)status);
	}
	else if (m_mode == kQuery)
	{
		status = doQuery();
		if (status)
		{
			if (m_isTitleSet)
				setResult(m_title.c_str());
			else if (m_isRectSet)
			{
				MDoubleArray res;
				res.setLength((unsigned)m_rect.size());
				for (unsigned int i = 0; i < m_rect.size(); ++i)
					res[i] = m_rect[i];
				setResult(res);
			}
			else if (m_isColorSet)
			{
				MIntArray res;
				res.setLength((unsigned)m_color.size());
				for (unsigned int i = 0; i < m_color.size(); ++i)
					res[i] = m_color[i];
				setResult(res);
			}
			else if (m_isTextSet)
				setResult(m_text.c_str());
		}
		else
			setResult(false);
	}

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Cleanup
#undef kErrorBothModeFlagsSet
#undef kErrorFlagNotSet
#undef kErrorParsingFlag
#undef kErrorNoTabInfoNode
#undef kErrorRetrievingTabInfoNode
#undef kErrorEditModeRequired
#undef kWarningIgnoreMemberFlags
#undef kErrorDefaultsRequired
#undef kErrorMemberFlagNotSet
#undef kErrorMultipleMemberFlagsOnQuery
#undef kErrorSetHandleData
#undef kErrorExistingStream
#undef kErrorCreateStream
#undef kErrorRetrieveStream
#undef kErrorRemovingStream

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
