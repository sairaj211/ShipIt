#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "CreateGameEvent.h"

CreateGameEvent::CreateGameEvent() : Event("CreateGameEvent")
{
	mOutput = "";
}

CreateGameEvent::~CreateGameEvent()
{

}

void CreateGameEvent::Reset()
{
	mOutput = "";

	Event::Reset();
}

CreateGameEvent* CreateGameEvent::Create()
{
	return new CreateGameEvent();
}
