#pragma once
#include "Event.h"

/*!
 * An event for when the resource associated
 * with a cell changes
 *
 */
class SetCellEvent : public Event
{
public:
	/*!
	 * SetCellEvent Default Constructor
	 *
	 */
	GEAR_API SetCellEvent();
	/*!
	 * SetCellEvent Default Destructor
	 *
	 */
	GEAR_API ~SetCellEvent();
	/*!
	 * Resets the SetCellEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new SetCellEvent
	 *
	 * \return A pointer to a new SetCellEvent
	 */
	GEAR_API virtual SetCellEvent* Create();

public:
	int mResource; /*!< Integer representation of the resource being associated with the cell */
	int mGridID; /*!< ID of the grid logic object that manages the cells */
};

