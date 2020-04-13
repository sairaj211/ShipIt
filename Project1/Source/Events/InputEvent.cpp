#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "InputEvent.h"

InputEvent::InputEvent() : Event("InputEvent")
{
	mGeneratorID = -1;
}

InputEvent::~InputEvent()
{

}

void InputEvent::Reset()
{
	mGeneratorID = -1;

	Event::Reset();
}

InputEvent* InputEvent::Create()
{
	return new InputEvent();
}