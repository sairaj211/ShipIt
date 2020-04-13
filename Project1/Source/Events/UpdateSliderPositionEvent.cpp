#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "UpdateSliderPositionEvent.h"

UpdateSliderPositionEvent::UpdateSliderPositionEvent() : Event("UpdateSliderPositionEvent")
{
	mSliderPosition = glm::vec3(0);
}

UpdateSliderPositionEvent::~UpdateSliderPositionEvent()
{

}

void UpdateSliderPositionEvent::Reset()
{
	mSliderPosition = glm::vec3(0);
	Event::Reset();
}

UpdateSliderPositionEvent* UpdateSliderPositionEvent::Create()
{
	return new UpdateSliderPositionEvent();
}