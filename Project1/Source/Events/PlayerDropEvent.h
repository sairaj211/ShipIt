#pragma once
#include "Event.h"

/*!
 * An event for when a Player drops
 * an item
 *
 */
class PlayerDropEvent : public Event
{
public:
	/*!
	 * PlayerDropEvent Default Constructor
	 *
	 */
	GEAR_API PlayerDropEvent();
	/*!
	 * PlayerDropEvent Default Destructor
	 *
	 */
	GEAR_API ~PlayerDropEvent();
	/*!
	 * Resets the PlayerDropEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new PlayerDropEvent
	 *
	 * \return A pointer to a new PlayerDropEvent
	 */
	GEAR_API virtual PlayerDropEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity that is dropping an item */
};

