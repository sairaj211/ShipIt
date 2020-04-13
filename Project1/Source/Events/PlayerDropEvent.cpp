#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "PlayerDropEvent.h"

PlayerDropEvent::PlayerDropEvent() : Event("PlayerDropEvent")
{
	mPlayerID = -1;
}

PlayerDropEvent::~PlayerDropEvent()
{

}

void PlayerDropEvent::Reset()
{
	mPlayerID = -1;

	Event::Reset();
}

PlayerDropEvent* PlayerDropEvent::Create()
{
	return new PlayerDropEvent();
}