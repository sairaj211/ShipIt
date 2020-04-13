#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "SetCellEvent.h"

SetCellEvent::SetCellEvent() : Event("SetCellEvent")
{
	mResource = -1;
	mGridID = -1;
}

SetCellEvent::~SetCellEvent()
{

}

void SetCellEvent::Reset()
{
	mResource = -1;
	mGridID = -1;

	Event::Reset();
}

SetCellEvent* SetCellEvent::Create()
{
	return new SetCellEvent();
}
