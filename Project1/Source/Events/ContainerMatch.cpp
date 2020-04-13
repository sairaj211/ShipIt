#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ContainerMatchEvent.h"

ContainerMatchEvent::ContainerMatchEvent() : Event("ContainerMatchEvent")
{
	mContainerID = -1;
}

ContainerMatchEvent::~ContainerMatchEvent()
{

}

void ContainerMatchEvent::Reset()
{
	mContainerID = -1;
	Event::Reset();
}

ContainerMatchEvent* ContainerMatchEvent::Create()
{
	return new ContainerMatchEvent();
}