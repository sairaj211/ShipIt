#pragma once
#include "Event.h"

/*!
 * An event for when a set of inputs
 * in a processor match a given recipe
 *
 */
class RecipeMatchEvent : public Event
{
public:
	/*!
	 * RecipeMatchEvent Default Constructor
	 *
	 */
	GEAR_API RecipeMatchEvent();
	/*!
	 * RecipeMatchEvent Default Destructor
	 *
	 */
	GEAR_API ~RecipeMatchEvent();
	/*!
	 * Resets the RecipeMatchEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new RecipeMatchEvent
	 *
	 * \return A pointer to a new RecipeMatchEvent
	 */
	GEAR_API virtual RecipeMatchEvent* Create();

public:
	int mProcessorID; /*!< ID of the Processor Entity who's input matches a recipe */
	int mOutput; /*!< Integer representing the resource being output */
};

