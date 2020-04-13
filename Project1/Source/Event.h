#pragma once

/*!
 * 
 * 
 */
class Event
{
public:
	/*!
	 * Constructor for the Abstract Event class
	 * 
	 * \param aType The Event type, should be the name of the event
	 * \param aTime The delay of the event, used if event is delayed
	 */
	GEAR_API Event(std::string aType, float aTime = 0.0f) : mRead(false), mType(aType), mTime(aTime) {};
	/*!
	 * Default Destructor
	 * 
	 */
	GEAR_API ~Event() {};
	/*!
	 * 
	 * Mark the Event as being Read
	 */
	GEAR_API void Read() { mRead = true; }
	/*!
	 * Get the event's type
	 * 
	 * \return The type string of the event
	 */
	GEAR_API std::string GetType() { return mType; }
	/*!
	 * Clear the Event of any previous data
	 * Type will be an empty string and time is set to zero
	 */
	GEAR_API virtual void Reset() { mRead = false; mTime = 0.0f; };
	/*!
	 * Create a new Event, type is empty ant time is zero
	 * 
	 * \return 
	 */
	GEAR_API virtual Event* Create() { return new Event("Default"); }
	/*!
	 * Set the time for the Event
	 * 
	 * \param aTime
	 */
	GEAR_API void SetTime(float aTime) { mTime = aTime; };
	/*!
	 * Get the time of the Event
	 * 
	 * \return 
	 */
	GEAR_API float GetTime() { return mTime; };
protected:
	bool mRead; /*!< If the Event has be read or not */
	float mTime; /*!< Amount of time for the event to fire */
	std::string mType; /*!< The Name of the Event */
};

