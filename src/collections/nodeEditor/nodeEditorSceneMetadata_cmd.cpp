#include "nodeEditorSceneMetadata_cmd.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
	NOTE
	----
	Commands contained within this file are no longer in use and have been replaced by those in the "nodeEditorTabInfoMetadata_cmd" file
	- It turns out scene-level metadata is not all that useful
	- When importing an external file which also contains scene-level metadata, the current file will have all of its scene-level metadata overwritten
	- This is due to Maya's import mechanism making use of the applyMetadata command which appears to overwrite channels at the scene-level
	- Whereas at the node-level the command does not appear to overwrite existing channels whilst also providing the benefit of namespacing
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Explicit Instantiations

template StructureTemplate<3>;
template StructureTemplate<4>;
template ChannelTemplate<3>;
template ChannelTemplate<4>;
template const adsk::Data::Structure* registerStructure(adsk::Data::Accessor* accessor, const StructureTemplate<3>& structureTemplate);
template const adsk::Data::Structure* registerStructure(adsk::Data::Accessor* accessor, const StructureTemplate<4>& structureTemplate);

// ------ Defines ------

#define kErrorBothModeFlagsSet \
	"Cannot specify \"-edit\" and \"-query\" flags simultaneously."

#define kErrorFlagNotSet \
	"The \"^1s\" flag is required by this command."

#define kErrorParsingFlag \
	"Error parsing flag \"^1s\"."

#define kErrorNoFile \
	"The current scene has not yet been saved, unable to write metadata. Use the \"^1s\" flag to provide an existing file path or save the current scene."

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

#define kErrorAccessorNotFound \
	"Cannot find accessor for file \"^1s\"."

#define kErrorReadFileMetadata \
	"Error reading metdata from file \"^1s\" (errors = ^2s)."

#define kErrorWriteFileMetadata \
	"Error writing metdata to file \"^1s\" (errors = ^2s)."

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

/*	Description
	-----------
	Constructs an Accessor which is compatible with the given filepath extension
	Reads all of the Structure data and the scene Association data for the given file into the Accessor object
	The returned pointer allows us to read and modify existing scene metadata
	Any changes that are made via the pointer must be written back into the scene by calling Accessor::write() to ensure they persist    */
std::unique_ptr<adsk::Data::Accessor> getSceneAccessor(const char* filePath)
{
	std::string errors;

	std::string stdFilePath(filePath);
	std::unique_ptr<adsk::Data::Accessor> accessor = adsk::Data::Accessor::accessorByExtension(stdFilePath);
	if (!accessor)
	{
		MString msg;
		MString msgFormat = kErrorAccessorNotFound;
		msg.format(msgFormat, filePath);
		MGlobal::displayError(msg);
		return nullptr;
	}

	// Read all Structures for the specific scene Association
	std::set<std::string> wantedAssociations;
	wantedAssociations.insert(adsk::Data::AccessorMaya::getSceneAssociationsName());
	if (!accessor->read(stdFilePath, nullptr, &wantedAssociations, errors))
	{
		MString msg;
		MString msgFormat = kErrorReadFileMetadata;
		msg.format(msgFormat, filePath, errors.c_str());
		MGlobal::displayError(msg);
		return nullptr;
	}
	return accessor;
}

/*	Description
	-----------
	Attempts to find and return the internal scene-level Associations for the given Accessor object
	This function expects to receive an Accessor which has been initialized to a specific file (ie. it has read the Association data for that file)
	If the scene-level Associations cannot be found, they will be created and stored in the Accessor's internal AssociationsMap    */
adsk::Data::Associations getSceneAssociations(adsk::Data::Accessor &accessor)
{
	adsk::Data::Associations associations;

	// Retrieve the scene-level Associations
	const std::map<std::string, adsk::Data::Associations> &associationsMap = accessor.associations();
	auto it = associationsMap.find(adsk::Data::AccessorMaya::getSceneAssociationsName());
	if (it == associationsMap.end())
	{
		// The scene-level Associations were not found, create them and store them in the Accessor's internal AssociationsMap using a non-const reference
		associations = adsk::Data::Associations::create(); // Implicit copy-constructor, object takes ownership of the dynamic allocation
		accessor.associations()[adsk::Data::AccessorMaya::getSceneAssociationsName()] = associations; // Register data (ownership is now shared, ie. ref count increased)
	}
	else
		associations = (*it).second;

	return associations;
}

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
	Ensures the Structure is registered with the given Accessor so that it can persist with the metadata of the associated file
	The new Structure will be modelled from the given Structure data, providing a description of the data required to serialize an entity with the scene
	The Structure will be written into the file associated with the command's Accessor after calling the Accessor::write() method    */
template<unsigned int N>
const adsk::Data::Structure* registerStructure(adsk::Data::Accessor* accessor, const StructureTemplate<N>& structureTemplate)
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

	// Make sure our Structure is registered in the Accessor's StructureSet since the Accessor's scene-level Association will be making use of it
	const std::set<const adsk::Data::Structure*, adsk::Data::Accessor::StructureNameLessThan>& structureSet = accessor->structures();
	if (structureSet.find(structure) == structureSet.end())
	{
		std::set<const adsk::Data::Structure*, adsk::Data::Accessor::StructureNameLessThan> updatedStructureSet = structureSet;
		updatedStructureSet.insert(structure);
		accessor->setStructures(updatedStructureSet);
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

/*	Description
	-----------
	Designed to modify the serialization of the current scene-level Associations data to prevent escaping of common sequences
	This then allows the result to be used with the applyMetadata command to update the metadata of the current scene    */
std::string parseSerialization(std::stringstream& serialization)
{
	std::stringstream parsed{ "" };

	char buffer;
	unsigned int pos = (unsigned)serialization.tellg();
	serialization.seekg(0, ios::end);
	unsigned int size = (unsigned)serialization.tellg();
	serialization.seekg(0, ios::beg);

	for (unsigned int i = 0; i < size; i++)
	{
		serialization.seekg(i);
		serialization.get(buffer);
		switch (buffer)
		{
		case '\"':
			parsed << "\\\""; break;
		case '\n':
			parsed << "\\n"; break;
		default:
			parsed << buffer; break;
		}
	}

	// Reset pointer to initial state
	serialization.seekg(pos);

	return parsed.str();
}

/*	Description
	-----------
	Function expects an Accessor which holds alterations to its internal Associations
	The alterations will be written to the file currently associated with the Accessor    */
MStatus writeAssociationsToFile(adsk::Data::Accessor* accessor)
{
	MStatus status;
	std::string errors;

	if (!accessor->write(errors))
	{
		MString msg;
		MString msgFormat = kErrorWriteFileMetadata;
		msg.format(msgFormat, accessor->fileName().c_str(), errors.c_str());
		MGlobal::displayError(msg);
		return MStatus::kFailure;
	}

	return status;
}

/*	Description
	-----------
	This function expects the scene-level Associations of the current file as well as an AssociationsSerializer capable of serializing them
	After writing metadata to the current file (ie. open in Maya) via an Accessor, it is necessary to take further measures to ensure the metadata persists
	If the user saves the current scene before reopening it, the metadata we have just written will be discarded (ie. overwritten by the previous values)
	Furthermore the metadata we have just written is not visible to the MEL metadata commands
	It has been written into the ASCII/binary file, however Maya does not seem to internally recognise the changes until the scene is reopened
	As a workaround, we can use the applyMetadata command to apply the current serialization to the scene-level metadata without needing to reopen
	It appears this updates the internal state such that the next time the scene is saved our changes will not revert    */
MStatus writeAssociationsToScene(adsk::Data::Associations associations, const adsk::Data::AssociationsSerializer* serializer)
{
	MStatus status;
	std::string errors;

	serializer = adsk::Data::AssociationsSerializer::formatByName("raw");
	std::stringstream writtenData;
	serializer->write(associations, writtenData, errors);
	std::string parsed = parseSerialization(writtenData);
	MString serialization{ parsed.c_str() };
	MString cmd = "applyMetadata -format \"raw\" -value \"" + serialization + "\" -scene;";
	status = MGlobal::executeCommandOnIdle(cmd);

	return MStatus::kSuccess;
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
	The mrs_NodeBox command writes metadata to the scene-level Associations of a given file, allowing NodeBox QGraphicsItems to persist between sessions in the Node Editor
	Specifically, this data is stored in the mrs_NodeBox_channel which is a descendant Channel of the scene-level Associations
	The Channel stores Streams which are templated from the mrs_NodeBox_structure, each Stream stores the data for a single QGraphicsItem (ie. one Element per Stream)
	In create mode this command enforces the proper initialization of a Stream by requiring all Member flags are specified (see flag documentation below)
	This command also supports both query and edit modes such that it is possible to edit/query Members of an existing Stream
	In edit mode, at least one Member flag must be specified and in query mode exactly one Member flag must be specified
	In the case that the user wants to update the metadata of the current scene, the -file flag must not be used

	MEL Command
	-----------
	mrs_NodeBox [-streamName string] [-file string] [-delete boolean] [-title string] [-rect floatArray] [-color intArray]

	Examples
	--------
	Create Mode :
		mrs_NodeBox -streamName "streamName" -file "C:\\myFile.mb" -title "NodeBox" -rect 0 0 500 500 -color 255 255 255 255
		mrs_NodeBox -streamName "streamName" -title "NodeBox" -rect 0 0 500 500 -color 255 255 255 255
	Edit Mode :
		mrs_NodeBox -e -streamName "streamName" -file "C:\\myFile.mb" -title "My New Title" -rect 0 0 300 300
		mrs_NodeBox -e -streamName "streamName" -title "My New Title"
		mrs_NodeBox -e -delete -streamName "streamName"
	Query Mode :
		mrs_NodeBox -q -streamName "streamName" -file "C:\\myFile.mb" -rect
		mrs_NodeBox -q -streamName "streamName" -title

	Flags
	-----
	-streamName(-sn)
		In create mode, this flag specifies the name of the stream to create
		In query and edit modes, this flag specifies the name of an existing Stream from which to retrieve or update Member data
		This flag is required in all modes

	-file(-f)
		This flag specifies the file path pointing to an existing ASCII/binary Maya file on which this command should operate
		If the metadata of the current scene needs to be changed, this flag must NOT be used

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
	m_isCurrentFile{ false },
	m_isDeleteEnabled{ false },
	m_isTitleSet{ false },
	m_isRectSet{ false },
	m_isColorSet{ false },
	m_accessor{ nullptr },
	m_structure{ nullptr },
	m_stream{ nullptr },
	m_serializer{ nullptr }{}

NodeBoxCommand::~NodeBoxCommand()
{
	m_accessor.reset();
	m_structure = nullptr;
	m_stream = nullptr;
	m_serializer = nullptr;
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
const char* NodeBoxCommand::kFilePathFlag = "-f";
const char* NodeBoxCommand::kFilePathFlagLong = "-file";
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
	syntax.addFlag(kFilePathFlag, kFilePathFlagLong, MSyntax::kString);
	syntax.addFlag(kDeleteFlag, kDeleteFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kTitleFlag, kTitleFlagLong, MSyntax::kString);
	syntax.addFlag(kRectFlag, kRectFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kColorFlag, kColorFlagLong, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned);

	syntax.makeFlagQueryWithFullArgs(kStreamNameFlag, false);
	syntax.makeFlagQueryWithFullArgs(kFilePathFlag, false);

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

	// Validate and parse -streamName/-file
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

	m_isCurrentFile = !argParser.isFlagSet(kFilePathFlagLong);
	if (m_isCurrentFile)
	{
		m_filePath = MGlobal::executeCommandStringResult("file -q -loc");
		if (m_filePath == "unknown")
		{
			MString msg;
			MString msgFormat = kErrorNoFile;
			msg.format(msgFormat, kFilePathFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	else
	{
		if (!argParser.getFlagArgument(kFilePathFlagLong, 0, m_filePath))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kFilePathFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
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
	
	m_structure = registerStructure<3>(m_accessor.get(), kChannelTemplate.structure);
	m_stream = registerStream(m_streamName.asChar(), kChannelTemplate.name, m_associations, m_structure);
	if (m_stream == nullptr)
		return MStatus::kFailure;

	registerElement(0, m_stream);
	if (!status)
		return status;

	status = editMembers();
	if (!status)
		return status;

	// Write any alterations made to the scene-level Associations into the file parsed by this command
	status = writeAssociationsToFile(m_accessor.get());
	if (!status)
		return status;
	if (m_isCurrentFile)
		status = writeAssociationsToScene(m_associations, m_serializer);

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

	// Write any alterations made to the scene-level Associations into the file parsed by this command
	status = writeAssociationsToFile(m_accessor.get());
	if (!status)
		return status;
	if (m_isCurrentFile)
		status = writeAssociationsToScene(m_associations, m_serializer);

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

	m_accessor = getSceneAccessor(m_filePath.asChar());
	if (!m_accessor)
	{
		setResult(false);
		return MStatus::kFailure;
	}

	m_associations = getSceneAssociations(*m_accessor.get());

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
	The mrs_Sticky command writes metadata to the scene-level Associations of a given file, allowing Sticky QGraphicsItems to persist between sessions in the Node Editor
	Specifically, this data is stored in the mrs_Sticky_channel which is a descendant Channel of the scene-level Associations
	The Channel stores Streams which are templated from the mrs_Sticky_structure, each Stream stores the data for a single QGraphicsItem (ie. one Element per Stream)
	In create mode this command enforces the proper initialization of a Stream by requiring all Member flags are specified (see flag documentation below)
	This command also supports both query and edit modes such that it is possible to edit/query Members of an existing Stream
	In edit mode, at least one Member flag must be specified and in query mode exactly one Member flag must be specified
	In the case that the user wants to update the metadata of the current scene, the -file flag must not be used

	MEL Command
	-----------
	mrs_Sticky [-streamName string] [-file string] [-delete boolean] [-title string] [-rect floatArray] [-color intArray] [-text string]

	Examples
	--------
	Create Mode :
		mrs_Sticky -streamName "streamName" -file "C:\\myFile.mb" -title "Sticky" -rect 0 0 500 500 -color 255 255 255 255 -text "This is a comment"
		mrs_Sticky -streamName "streamName" -title "Sticky" -rect 0 0 500 500 -color 255 255 255 255 -text "This is a comment"
	Edit Mode :
		mrs_Sticky -e -streamName "streamName" -file "C:\\myFile.mb" -title "My New Title" -rect 0 0 300 300
		mrs_Sticky -e -streamName "streamName" -title "My New Title"
		mrs_Sticky -e -delete -streamName "streamName"
	Query Mode :
		mrs_Sticky -q -streamName "streamName" -file "C:\\myFile.mb" -rect
		mrs_Sticky -q -streamName "streamName" -title

	Flags
	-----
	-streamName(-sn)
		In create mode, this flag specifies the name of the stream to create
		In query and edit modes, this flag specifies the name of an existing Stream from which to retrieve or update Member data
		This flag is required in all modes

	-file(-f)
		This flag specifies the file path pointing to an existing ASCII/binary Maya file on which this command should operate
		If the metadata of the current scene needs to be changed, this flag must NOT be used

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
	m_isCurrentFile{ false },
	m_isDeleteEnabled{ false },
	m_isTitleSet{ false },
	m_isRectSet{ false },
	m_isColorSet{ false },
	m_isTextSet{ false },
	m_accessor{ nullptr },
	m_structure{ nullptr },
	m_stream{ nullptr },
	m_serializer{ nullptr }{}

StickyCommand::~StickyCommand()
{
	m_accessor.reset();
	m_structure = nullptr;
	m_stream = nullptr;
	m_serializer = nullptr;
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
const char* StickyCommand::kFilePathFlag = "-f";
const char* StickyCommand::kFilePathFlagLong = "-file";
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
	syntax.addFlag(kFilePathFlag, kFilePathFlagLong, MSyntax::kString);
	syntax.addFlag(kDeleteFlag, kDeleteFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kTitleFlag, kTitleFlagLong, MSyntax::kString);
	syntax.addFlag(kRectFlag, kRectFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kColorFlag, kColorFlagLong, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned, MSyntax::kUnsigned);
	syntax.addFlag(kTextFlag, kTextFlagLong, MSyntax::kString);

	syntax.makeFlagQueryWithFullArgs(kStreamNameFlag, false);
	syntax.makeFlagQueryWithFullArgs(kFilePathFlag, false);

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

	// Validate and parse -streamName/-file
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

	m_isCurrentFile = !argParser.isFlagSet(kFilePathFlagLong);
	if (m_isCurrentFile)
	{
		m_filePath = MGlobal::executeCommandStringResult("file -q -loc");
		if (m_filePath == "unknown")
		{
			MString msg;
			MString msgFormat = kErrorNoFile;
			msg.format(msgFormat, kFilePathFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}
	else
	{
		if (!argParser.getFlagArgument(kFilePathFlagLong, 0, m_filePath))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kFilePathFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
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
	
	m_structure = registerStructure<4>(m_accessor.get(), kChannelTemplate.structure);
	m_stream = registerStream(m_streamName.asChar(), kChannelTemplate.name, m_associations, m_structure);
	if (m_stream == nullptr)
		return MStatus::kFailure;

	registerElement(0, m_stream);
	if (!status)
		return status;

	status = editMembers();
	if (!status)
		return status;

	// Write any alterations made to the scene-level Associations into the file parsed by this command
	status = writeAssociationsToFile(m_accessor.get());
	if (!status)
		return status;
	if (m_isCurrentFile)
		status = writeAssociationsToScene(m_associations, m_serializer);

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

	// Write any alterations made to the scene-level Associations into the file parsed by this command
	status = writeAssociationsToFile(m_accessor.get());
	if (!status)
		return status;
	if (m_isCurrentFile)
		status = writeAssociationsToScene(m_associations, m_serializer);

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

	m_accessor = getSceneAccessor(m_filePath.asChar());
	if (!m_accessor)
	{
		setResult(false);
		return MStatus::kFailure;
	}

	m_associations = getSceneAssociations(*m_accessor.get());

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
#undef kErrorNoFile
#undef kErrorEditModeRequired
#undef kWarningIgnoreMemberFlags
#undef kErrorDefaultsRequired
#undef kErrorMemberFlagNotSet
#undef kErrorMultipleMemberFlagsOnQuery
#undef kErrorAccessorNotFound
#undef kErrorReadFileMetadata
#undef kErrorWriteFileMetadata
#undef kErrorSetHandleData
#undef kErrorExistingStream
#undef kErrorCreateStream
#undef kErrorRetrieveStream

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
