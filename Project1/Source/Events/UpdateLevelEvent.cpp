#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "UpdateLevelEvent.h"

UpdateLevelEvent::UpdateLevelEvent() : Event("UpdateLevelEvent")
{
	mCurrentTime = 0.0f;
}

UpdateLevelEvent::~UpdateLevelEvent()
{

}

void UpdateLevelEvent::Reset()
{
	mCurrentTime = 0.0f;
	Event::Reset();
}

UpdateLevelEvent* UpdateLevelEvent::Create()
{
	return new UpdateLevelEvent();
}