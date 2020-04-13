#pragma once
#include "Resource.h"

/*!
 * Text Resource that defines a text
 * file hierarchy
 *
 */
class Text : public Resource
{
public:
	/*!
	 * Text Overloaded Constructor
	 *
	 * \param a_name The name of the Text Resource
	 */
	GEAR_API Text(std::string a_name);
	/*!
	 * Text Default Destructor
	 *
	 */
	GEAR_API virtual ~Text();
	/*!
	 * Load the Text with Json
	 *
	 */
	GEAR_API void Load();

public:
	std::fstream mFileStream; /*!< File stream corresponding to a text file */
};