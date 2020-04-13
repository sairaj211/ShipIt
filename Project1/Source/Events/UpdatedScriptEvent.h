#pragma once

#include "Event.h"
/*!
 * 
 * \brief event to tell script component update or add a script
 */
class UpdatedScriptEvent :
	public Event
{
public:
	/*!
	 * Create a new updated script event
	 * 
	 * \param _file the file that changed
	 * \param _update the type of update
	 */
	GEAR_API UpdatedScriptEvent(std::string _file ="",std::string _update="");
	/*!
	 * Default dtor
	 * 
	 */
	GEAR_API ~UpdatedScriptEvent();
	/*!
	 * Get the type of update
	 * 
	 * \return string for the kind of update
	 */
	GEAR_API std::string GetUpdate();
	/*!
	 * Get the file path
	 * 
	 * \return file path
	 */
	GEAR_API std::string GetFile();
	/*!
	 * Set the update for the event
	 * 
	 * \param _update value to set
	 */
	GEAR_API void SetUpdate(std::string _update);
	/*!
	 * Set file for the event
	 * 
	 * \param _file value to set, should be a path
	 */
	GEAR_API void SetFile(std::string _file);
	/*!
	 * 
	 * Clear all data
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Create a new black scripting evetn
	 * 
	 * \return new UpdateScripting event
	 */
	GEAR_API virtual UpdatedScriptEvent* Create() { return new UpdatedScriptEvent(); };
private:
	std::string mUpdateType;
	std::string mFileName;
};

