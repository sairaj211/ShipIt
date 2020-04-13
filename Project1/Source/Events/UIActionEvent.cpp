#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "UIActionEvent.h"

UIActionEvent::UIActionEvent() : Event("UIActionEvent")
{
}

UIActionEvent::~UIActionEvent()
{

}

void UIActionEvent::Reset()
{
	Event::Reset();
}

UIActionEvent* UIActionEvent::Create()
{
	return new UIActionEvent();
}