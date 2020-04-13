#pragma once
#include "Event.h"

/*!
 * An event for when a Player has input
 * the correct sequence and the Generator
 * needs to create an output
 *
 */
class GeneratorOutputEvent : public Event
{
public:
	/*!
	 * GeneratorOutputEvent Default Constructor
	 *
	 */
	GEAR_API GeneratorOutputEvent();
	/*!
	 * GeneratorOutputEvent Default Destructor
	 *
	 */
	GEAR_API ~GeneratorOutputEvent();
	/*!
	 * Resets the GeneratorOutputEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new GeneratorOutputEvent
	 *
	 * \return A pointer to a new GeneratorOutputEvent
	 */
	GEAR_API virtual GeneratorOutputEvent* Create();

public:
	int mPlayerID; /*!< ID of the Player Entity who is requesting the output */
	int mGeneratorID; /*!< ID of the Generator Entity that is generating the output */
	std::vector<std::string> mOutputs; /*!< List of file names corresponding to the output Entities */
};

