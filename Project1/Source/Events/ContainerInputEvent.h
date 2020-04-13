#pragma once
#include "Event.h"

/*!
 * An event for when a Player tries to
 * add a number of items to a Container
 *
 */
class ContainerInputEvent : public Event
{
public:
	/*!
	 * ContainerInputEvent Default Constructor
	 *
	 */
	GEAR_API ContainerInputEvent();
	/*!
	 * ContainerInputEvent Default Destructor
	 *
	 */
	GEAR_API ~ContainerInputEvent();
	/*!
	 * Resets the ContainerInputEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new ContainerInputEvent
	 *
	 * \return A pointer to a new ContainerInputEvent
	 */
	GEAR_API virtual ContainerInputEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity adding items to the Container */
	int mContainerID; /*!< ID of the Container Entity that the items are being added to */
	int mNumItems; /*!< Number of items being added to the Container */
};

