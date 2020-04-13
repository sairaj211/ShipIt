#pragma once
#include "Event.h"

/*!
 * An event for when the grid recipe
 * matches and a game is being created
 *
 */
class CreateGameEvent : public Event
{
public:
	/*!
	 * CreateGameEvent Default Constructor
	 *
	 */
	GEAR_API CreateGameEvent();
	/*!
	 * CreateGameEvent Default Destructor
	 *
	 */
	GEAR_API ~CreateGameEvent();
	/*!
	 * Resets the CreateGameEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new CreateGameEvent
	 *
	 * \return A pointer to a new CreateGameEvent
	 */
	GEAR_API virtual CreateGameEvent* Create();

public:
	std::string mOutput; /*!< Name of the game being output */
};

