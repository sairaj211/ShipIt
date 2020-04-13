#pragma once
#include "Core.h"

/*!
 * Base Resource that defines the functions
 * and member data expected in each Resource
 *
 */
class Resource
{
public:
	/*!
	 * Resource Overloaded Constructor
	 * 
	 * \param a_name Name of the Resource
	 */
	GEAR_API Resource(std::string a_name);
	/*!
	 * Resource Default Destructor
	 * 
	 */
	GEAR_API virtual ~Resource();
	/*!
	 * Reset the Resource so it can be reloaded
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Loads the Resource according to its type
	 * 
	 */
	GEAR_API virtual void Load() = 0;
	/*!
	 * Gets flag that indicates load failure
	 * 
	 * \return Flag that is true if prevous Load() call was unsuccessful, false otherwise
	 */
	GEAR_API bool LoadFailed();

public:
	std::string mName; /*!< Name of the Resource */

protected:
	bool mLoadFailed; /*!< Flag that indicates load failure */
};