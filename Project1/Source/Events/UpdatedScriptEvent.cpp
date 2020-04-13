#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "UpdatedScriptEvent.h"

UpdatedScriptEvent::UpdatedScriptEvent(std::string _file, std::string _update) : Event("UpdatedScriptEvent"), mFileName(_file), mUpdateType(_update)
{
}

UpdatedScriptEvent::~UpdatedScriptEvent()
{
}

std::string UpdatedScriptEvent::GetUpdate()
{
	return mUpdateType;
}

std::string UpdatedScriptEvent::GetFile()
{
	return mFileName;
}

void UpdatedScriptEvent::SetUpdate(std::string _update)
{
	mUpdateType = _update;
}

void UpdatedScriptEvent::SetFile(std::string _file)
{
	mFileName = _file;
}

void UpdatedScriptEvent::Reset()
{
	mRead = false;
	mType = "UpdatedScriptEvent";
	mTime = 0.0f;
	mUpdateType = "";
	mFileName = "";
}
