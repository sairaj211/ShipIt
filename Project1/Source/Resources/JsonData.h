#pragma once
#include "Resource.h"
/*!
 * JsonData Resource that defines a Json
 * file hierarchy
 * 
 */
class JsonData : public Resource
{
public:
	/*!
	 * JsonData Overloaded Constructor
	 * 
	 * \param a_name The name of the JsonData Resource
	 * \param a_path The path of the specified Json file
	 */
	GEAR_API JsonData(std::string a_name, std::string a_path);
	/*!
	 * JsonData Default Destructor
	 * 
	 */
	GEAR_API virtual ~JsonData();
	/*!
	 * Load the JsonData with Json
	 * 
	 */
	GEAR_API void Load();

public:
	Json::Value mRoot; /*!< Root node of the Json hierarchy */
	std::string mPath; /*!< Relative path to the Json file */
};