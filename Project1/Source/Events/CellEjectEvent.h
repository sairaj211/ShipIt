

#pragma once
#include "Event.h"

/*!
 * An event for when the contents of a
 * cell are to be ejected
 *
 */
class CellEjectEvent : public Event
{
public:
	/*!
	 * CellEjectEvent Default Constructor
	 *
	 */
	GEAR_API CellEjectEvent();
	/*!
	 * CellEjectEvent Default Destructor
	 *
	 */
	GEAR_API ~CellEjectEvent();
	/*!
	 * Creates a pointer to a new CellEjectEvent
	 *
	 * \return A pointer to a new CellEjectEvent
	 */
	GEAR_API virtual CellEjectEvent* Create();
};

