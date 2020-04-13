#pragma once
#include "Event.h"

/*!
 * An event for when a Request is completed
 *
 */
class RequestFilledEvent : public Event
{
public:
	/*!
	 * RequestFilledEvent Default Constructor
	 *
	 */
	GEAR_API RequestFilledEvent();
	/*!
	 * RequestFilledEvent Default Destructor
	 *
	 */
	GEAR_API ~RequestFilledEvent();
	/*!
	 * Resets the RequestFilledEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new RequestFilledEvent
	 *
	 * \return A pointer to a new RequestFilledEvent
	 */
	GEAR_API virtual RequestFilledEvent* Create();

public:
	int mContainerID; /*!< ID of the Container Entity that the request was completed for */
	int mRequestedGame; /*!< Integer representation of the game that was requested */
};

