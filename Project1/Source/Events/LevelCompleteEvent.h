#pragma once
#include "Event.h"

/*!
 * An event for when a Level is completed
 *
 */
class LevelCompleteEvent : public Event
{
public:
	/*!
	 * LevelCompleteEvent Default Constructor
	 *
	 */
	GEAR_API LevelCompleteEvent();
	/*!
	 * LevelCompleteEvent Default Destructor
	 *
	 */
	GEAR_API ~LevelCompleteEvent();
	/*!
	 * Resets the LevelCompleteEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new LevelCompleteEvent
	 *
	 * \return A pointer to a new LevelCompleteEvent
	 */
	GEAR_API virtual LevelCompleteEvent* Create();

public:
	float mScore; /*!< ID of the Container Entity that the request was completed for */
	int mRequestedGame; /*!< Integer representation of the game that was requested */
	std::string mGrade;
};

