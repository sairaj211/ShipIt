#pragma once
#include "Event.h"

/*!
 * An event for when the contents of a
 * cell change from matching to not
 * matching or vice versa
 *
 */
class CellUpdateEvent : public Event
{
public:
	/*!
	 * CellUpdateEvent Default Constructor
	 *
	 */
	GEAR_API CellUpdateEvent();
	/*!
	 * CellUpdateEvent Default Destructor
	 *
	 */
	GEAR_API ~CellUpdateEvent();
	/*!
	 * Resets the CellUpdateEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new CellUpdateEvent
	 *
	 * \return A pointer to a new CellUpdateEvent
	 */
	GEAR_API virtual CellUpdateEvent* Create();

public:
	bool mMatch; /*!< Flag indicating whether the contents of the cell match the recipe */
	int mCellID; /*!< ID of the cell in the grid */
	int mPlayerID; /*!< ID of the player that updated the grid cell */
};