#pragma once
#include "Event.h"

/*!
 * An event for when the grid cells are
 * to be re-enabled
 *
 */
class ResetGridEvent : public Event
{
public:
	/*!
	 * ResetGridEvent Default Constructor
	 *
	 */
	GEAR_API ResetGridEvent();
	/*!
	 * ResetGridEvent Default Destructor
	 *
	 */
	GEAR_API ~ResetGridEvent();
	/*!
	 * Creates a pointer to a new ResetGridEvent
	 *
	 * \return A pointer to a new ResetGridEvent
	 */
	GEAR_API virtual ResetGridEvent* Create();
};

