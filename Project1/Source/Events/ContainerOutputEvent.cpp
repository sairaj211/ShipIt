#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ContainerOutputEvent.h"

ContainerOutputEvent::ContainerOutputEvent() : Event("ContainerOutputEvent")
{
	mPlayerID = -1;
	mContainerID = -1;
	mNumItems = 0;
	mItemTypes.clear();
}

ContainerOutputEvent::~ContainerOutputEvent()
{

}

void ContainerOutputEvent::Reset()
{
	mPlayerID = -1;
	mContainerID = -1;
	mNumItems = 0;
	mItemTypes.clear();

	Event::Reset();
}

ContainerOutputEvent* ContainerOutputEvent::Create()
{
	return new ContainerOutputEvent();
}