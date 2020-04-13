#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "RequestMadeEvent.h"

RequestMadeEvent::RequestMadeEvent() : Event("RequestMadeEvent")
{
	mGame = -1;
}

RequestMadeEvent::~RequestMadeEvent()
{

}

void RequestMadeEvent::Reset()
{
	mGame = -1;

	Event::Reset();
}

RequestMadeEvent* RequestMadeEvent::Create()
{
	return new RequestMadeEvent();
}