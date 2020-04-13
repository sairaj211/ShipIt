#pragma once
#include "Event.h"

/*!
 * An event for when a Player tries to
 * remove a number of items from a Container
 *
 */
class ContainerOutputEvent : public Event
{
public:
	/*!
	 * ContainerOutputEvent Default Constructor
	 *
	 */
	GEAR_API ContainerOutputEvent();
	/*!
	 * ContainerOutputEvent Default Destructor
	 *
	 */
	GEAR_API ~ContainerOutputEvent();
	/*!
	 * Resets the ContainerOutputEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new ContainerOutputEvent
	 *
	 * \return A pointer to a new ContainerOutputEvent
	 */
	GEAR_API virtual ContainerOutputEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity removing items from the Container */
	int mContainerID; /*!< ID of the Container Entity that the items are being removed from */
	int mNumItems; /*!< Number of items being removed from the Container */
	std::vector<std::string> mItemTypes; /*!< Type of items being removed from the Container */
};

