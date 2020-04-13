#pragma once
#include "Event.h"

/*!
 * An event for when a Player has requested
 * output using a specific input button
 *
 */
class GeneratorInputEvent : public Event
{
public:
	/*!
	 * GeneratorInputEvent Default Constructor
	 *
	 */
	GEAR_API GeneratorInputEvent();
	/*!
	 * GeneratorInputEvent Default Destructor
	 *
	 */
	GEAR_API ~GeneratorInputEvent();
	/*!
	 * Resets the GeneratorInputEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new GeneratorInputEvent
	 *
	 * \return A pointer to a new GeneratorInputEvent
	 */
	GEAR_API virtual GeneratorInputEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity who has made the input */
	int mGeneratorID; /*!< ID of the Generator Entity that is taking the input */
	std::string mInputType; /*!< Type of input made by the Player */
};

