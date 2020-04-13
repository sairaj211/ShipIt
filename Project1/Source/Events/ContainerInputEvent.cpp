#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ContainerInputEvent.h"

ContainerInputEvent::ContainerInputEvent() : Event("ContainerInputEvent")
{
	mPlayerID = -1;
	mContainerID = -1;
	mNumItems = 0;
}

ContainerInputEvent::~ContainerInputEvent()
{

}

void ContainerInputEvent::Reset()
{
	mPlayerID = -1;
	mContainerID = -1;
	mNumItems = 0;

	Event::Reset();
}

ContainerInputEvent* ContainerInputEvent::Create()
{
	return new ContainerInputEvent();
}