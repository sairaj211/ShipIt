#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "RequestFilledEvent.h"

RequestFilledEvent::RequestFilledEvent() : Event("RequestFilledEvent")
{
	mContainerID = -1;
	mRequestedGame = -1;
}

RequestFilledEvent::~RequestFilledEvent()
{

}

void RequestFilledEvent::Reset()
{
	mContainerID = -1;
	mRequestedGame = -1;

	Event::Reset();
}

RequestFilledEvent* RequestFilledEvent::Create()
{
	return new RequestFilledEvent();
}