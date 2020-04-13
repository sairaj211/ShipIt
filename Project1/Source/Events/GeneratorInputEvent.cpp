#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "GeneratorInputEvent.h"

GeneratorInputEvent::GeneratorInputEvent() : Event("GeneratorInputEvent")
{
	mGeneratorID = -1;
	mPlayerID = -1;
	mInputType = "";
}

GeneratorInputEvent::~GeneratorInputEvent()
{

}

void GeneratorInputEvent::Reset()
{
	mGeneratorID = -1;
	mPlayerID = -1;
	mInputType = "";

	Event::Reset();
}

GeneratorInputEvent* GeneratorInputEvent::Create()
{
	return new GeneratorInputEvent();
}