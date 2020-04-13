#pragma once
#include "Event.h"

/*!
 * An event for when a Container meets
 * its requirements
 *
 */
class ContainerMatchEvent : public Event
{
public:
	/*!
	 * ContainerMatchEvent Default Constructor
	 *
	 */
	GEAR_API ContainerMatchEvent();
	/*!
	 * ContainerMatchEvent Default Destructor
	 *
	 */
	GEAR_API ~ContainerMatchEvent();
	/*!
	 * Resets the ContainerMatchEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new ContainerMatchEvent
	 *
	 * \return A pointer to a new ContainerMatchEvent
	 */
	GEAR_API virtual ContainerMatchEvent* Create();

public:
	int mContainerID; /*!< ID of the Container Entity that has met its requirements*/
};

