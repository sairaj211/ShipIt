#pragma once
#include "Event.h"

/*!
 * An event for when the correct input has
 * been made to the Generator
 *
 */
class InputEvent : public Event
{
public:
	/*!
	 * InputEvent Default Constructor
	 *
	 */
	GEAR_API InputEvent();
	/*!
	 * CorrectInputEvent Default Destructor
	 *
	 */
	GEAR_API ~InputEvent();
	/*!
	 * Resets the CorrectInputEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new CorrectInputEvent
	 *
	 * \return A pointer to a new CorrectInputEvent
	 */
	GEAR_API virtual InputEvent* Create();

public:
	int mGeneratorID; /*!< ID of the Generator Entity that the input has been made to */
	std::string mButton;
	int value;//1 pressed or joystick value

};

