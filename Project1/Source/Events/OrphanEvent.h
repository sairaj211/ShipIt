#pragma once
#include "Event.h"

/*!
 * An event for when an Entity loses
 * a child Entity
 *
 */
class OrphanEvent : public Event
{
public:
	/*!
	 * OrphanEvent Default Constructor
	 *
	 */
	GEAR_API OrphanEvent();
	/*!
	 * OrphanEvent Default Destructor
	 *
	 */
	GEAR_API ~OrphanEvent();
	/*!
	 * Resets the OrphanEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new OrphanEvent
	 *
	 * \return A pointer to a new OrphanEvent
	 */
	GEAR_API virtual OrphanEvent* Create();

public:
	int mParentID; /*!< ID of the parent Entity who lost a child Entity */
	int mChildID; /*!< ID of the child Entity who lost a parent Entity */
};

