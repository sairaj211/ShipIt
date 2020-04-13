#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "PlayerPickupEvent.h"

PlayerPickupEvent::PlayerPickupEvent() : Event("PlayerPickupEvent")
{
	mPlayerID = -1;
	mItemID = -1;
}

PlayerPickupEvent::~PlayerPickupEvent()
{

}

void PlayerPickupEvent::Reset()
{
	mPlayerID = -1;
	mItemID = -1;

	Event::Reset();
}

PlayerPickupEvent* PlayerPickupEvent::Create()
{
	return new PlayerPickupEvent();
}