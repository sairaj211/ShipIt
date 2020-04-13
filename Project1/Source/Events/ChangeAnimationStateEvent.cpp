#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ChangeAnimationStateEvent.h"

ChangeAnimationStateEvent::ChangeAnimationStateEvent() : Event("ChangeAnimationStateEvent")
{
	mState = "None";
	mID = -1;
}

ChangeAnimationStateEvent::~ChangeAnimationStateEvent()
{

}

void ChangeAnimationStateEvent::Reset()
{
	mState = "None";
	mID = -1;

	Event::Reset();
}

ChangeAnimationStateEvent* ChangeAnimationStateEvent::Create()
{
	return new ChangeAnimationStateEvent();
}
