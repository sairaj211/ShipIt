#pragma once
#include "Event.h"

/*!
 * An event for when a Player picks up
 * an item
 *
 */
class PlayerPickupEvent : public Event
{
public:
	/*!
	 * PlayerPickupEvent Default Constructor
	 *
	 */
	GEAR_API PlayerPickupEvent();
	/*!
	 * PlayerPickupEvent Default Destructor
	 *
	 */
	GEAR_API ~PlayerPickupEvent();
	/*!
	 * Resets the PlayerPickupEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new PlayerPickupEvent
	 *
	 * \return A pointer to a new PlayerPickupEvent
	 */
	GEAR_API virtual PlayerPickupEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity that is picking up an item */
	int mItemID; /*!< ID of the Item Entity that is picked up */
};

