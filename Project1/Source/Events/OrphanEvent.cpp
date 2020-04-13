#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "OrphanEvent.h"

OrphanEvent::OrphanEvent() : Event("OrphanEvent")
{
	mParentID = -1;
	mChildID = -1;
}

OrphanEvent::~OrphanEvent()
{

}

void OrphanEvent::Reset()
{
	mParentID = -1;
	mChildID = -1;

	Event::Reset();
}

OrphanEvent* OrphanEvent::Create()
{
	return new OrphanEvent();
}
