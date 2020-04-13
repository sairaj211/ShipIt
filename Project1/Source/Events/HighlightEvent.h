

#pragma once
#include "Event.h"

/*!
 * An event for when the contents of a
 * player is highlighting an Entity
 *
 */
class HighlightEvent : public Event
{
public:
	/*!
	 * HighlightEvent Default Constructor
	 *
	 */
	GEAR_API HighlightEvent();
	/*!
	 * HighlightEvent Default Destructor
	 *
	 */
	GEAR_API ~HighlightEvent();
	/*!
	 * Creates a pointer to a new HighlightEvent
	 *
	 * \return A pointer to a new HighlightEvent
	 */
	GEAR_API virtual HighlightEvent* Create();
};

