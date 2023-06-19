/*	
	Original work Copyright(c) 2016 Ryan Porter
	Modified work Copyright(c) 2018 Mitchell Simmons
	You may use, distribute, or modify this code under the terms of the MIT license.
*/

#pragma once

#include <vector>
#include <istream>
#include <ostream>

#include <maya/MArgList.h>
#include <maya/MPxData.h>
#include <maya/MStatus.h>
#include <maya/MStringArray.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Helpers ------
MStatus parseMString(const MString& str, short& outValue);
MStatus parseMString(const MString& str, int& outValue);
MStatus parseMString(const MString& str, float& outValue);
MStatus parseMString(const MString& str, double& outValue);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Provides overrides of MPxData methods which form a standard interface to be used with custom data types whose components are any single/multi-value data structures
// Instantiation is reserved for derived classes, therefore it can be assumed that methods are invoked from a derived object (or a pointer/reference with a dynamic type)
template<typename TElement, typename TData, unsigned int Size>
class DataArrayHelper : public MPxData
{
	// Types are limited to parsable MString types
	static_assert(
		std::is_same<TData, short>::value || 
		std::is_same<TData, int>::value ||
		std::is_same<TData, float>::value ||
		std::is_same<TData, double>::value,
		"DataArrayHelper : template is valid for 'TData' parameter types [short, int, float, double]");

	//static_assert(
	//	Size > 1, 
	//	"DataArrayHelper : template is valid for 'Size' parameter greater than 1 (ie. multi-value arrays)");

public:
	// ------ Helpers ------
	// Choosing to use an out-parameter in case we are dealing with large arrays which we don't want to reallocate
	virtual void getArray(std::vector<TElement>& outArray) const = 0;
	
	virtual void setArray(const std::vector<TElement>& array) = 0;

	virtual unsigned int length() const
	{
		return (unsigned int)m_data.size();
	}

	// ------ MPxData ------
	// Copies the underlying data between instances (eg. when an input and output attribute are connected)
	void copy(const MPxData& other) override
	{
		// typeId is pure virtual and will be called from the derived class
		if (typeId() == other.typeId())
		{
			// The MPxData reference retains a dynamic type (the inheriting class) to which accessor calls will be dispatched (casting will only change the static type)
			const DataArrayHelper<TElement, TData, Size>& otherData = (const DataArrayHelper<TElement, TData, Size>&)other;
			// Access the base class, else the virtual override will be called if it exists
			m_data.resize(otherData.DataArrayHelper<TElement, TData, Size>::length());
			std::copy(
				otherData.m_data.begin(),
				otherData.m_data.end(),
				m_data.begin()
			);
		}
	}

	// Parses ASCII data from the MArglist and stores it with the data member
	// On invocation the lastParsedIndex points to the last argument already parsed whilst on return it points to the last argument parsed by this method
	// For Python setAttr, the user will pass multi-element data using tuples: cmds.setAttr("node.eulerArrayAttr", 2,(1,1,1),(2,2,2), type='mrs_EulerArray')
	// For MEL setAttr, the user will pass multi-element data as individual values: setAttr node.eulerArrayAttr -type mrs_EulerArray 2 1 1 1 2 2 2;
	// For file open ASCII (.ma) operations, the method will be invoked with individual values
	MStatus readASCII(const MArgList& args, unsigned int& lastParsedIndex) override
	{
		MStatus status;

		unsigned int argLength = args.length();
		if (argLength == 0)
			return MStatus::kFailure;

		// The first argument should always be an integer specifying the number of arrays to be parsed
		unsigned int numArrays = (unsigned)args.asInt(lastParsedIndex++, &status);
		unsigned int numArgs = argLength - lastParsedIndex;
		unsigned int numValues = Size * numArrays;
		// One of these conditions must be true otherwise data is invalid
		bool arrayAsTuples = numArgs == numArrays;
		bool arrayAsValues = numArgs == (numArrays * Size);

		if (status && (arrayAsTuples || arrayAsValues))
		{
			m_data.resize(numValues);
			unsigned int valuesParsed = 0;

			if (numArrays > 0)
			{
				// Check asValues first in case the Size parameter is 1 in which case both asTuples and asValues will evaluate to true
				if (arrayAsValues)
				{
					for (unsigned int i = 0; i < numArgs; i++)
					{
						MString arg = args.asString(lastParsedIndex++, &status);

						if (status)
						{
							TData value;
							status = parseMString(arg, value);
							if (status)
							{
								m_data[i] = value;
								valuesParsed++;
							}
						}
					}
				}
				else
				{
					for (unsigned int i = 0; i < numArrays; i++)
					{
						unsigned int index = lastParsedIndex;
						// asStringArray will return a MStringArray at the argument position index + 1
						MStringArray argStr = args.asStringArray(index, &status);

						if (status)
						{
							for (unsigned int j = 0; j < Size; j++)
							{
								TData value;
								MString str = argStr[j];
								status = parseMString(str, value);
								if (status)
								{
									m_data[(i * Size) + j] = value;
									valuesParsed++;
								}
							}
							lastParsedIndex++;
						}
					}
				}
			}

			return valuesParsed == numValues ? MStatus::kSuccess : MStatus::kFailure;
		}

		return MStatus::kFailure;
	}

	// Parses values individually with the first value representing the integer number of arrays
	// Invoked for file open binary (.mb) operations
	MStatus readBinary(std::istream& in, unsigned int length) override
	{
		MStatus status;

		if (length == 0)
			return MStatus::kFailure;

		unsigned int numArrays;
		in.read((char*)&numArrays, sizeof(int));

		if (in.fail()) 
			status = MStatus::kFailure;
		else
		{
			unsigned int numValues = numArrays * Size;
			m_data.resize(numValues);

			for (unsigned int i = 0; i < numValues; i++)
			{
				in.read((char*)&m_data[i], sizeof(TData));

				if (in.fail())
					return MStatus::kFailure;
			}
		}

		return status;
	}

	// Writes out the number of arrays constituting the custom data structure then each value from each array delineated by a space
	// Invoked for file save ASCII (.ma) operations
	MStatus writeASCII(std::ostream &out) override
	{
		MStatus status;

		unsigned int numValues = this->DataArrayHelper<TElement, TData, Size>::length();
		unsigned int numArrays = numValues / Size;

		out << numArrays << " ";

		if (out.fail())
			status = MStatus::kFailure;
		else 
		{
			for (unsigned int i = 0; i < numValues; i++)
			{
				out << m_data[i] << " ";

				if (out.fail())
				{
					status = MStatus::kFailure;
					break;
				}
			}
		}

		return status;
	}

	// This is a direct reflection of readBinary, writing out the number of arrays then each value held by the data member
	// Invoked for file save binary (.mb) operations
	MStatus writeBinary(std::ostream &out) override
	{
		MStatus status;

		unsigned int numValues = this->DataArrayHelper<TElement, TData, Size>::length();
		unsigned int numArrays = numValues / Size;

		out.write((char*)&numArrays, sizeof(int));

		if (out.fail())
			status = MStatus::kFailure;
		else 
		{
			for (unsigned int i = 0; i < numValues; i++)
			{
				out.write((char*)&m_data[i], sizeof(TData));

				if (out.fail())
				{
					status = MStatus::kFailure;
					break;
				}
			}
		}

		return status;
	}

protected:
	DataArrayHelper() {}
	~DataArrayHelper() {}

	// ------ Data ------
	std::vector<TData> m_data;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------