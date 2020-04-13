#include "pch.h"
#include "Generator.h"
#include "pch.h"
#include "Managers/Log.h"


Generator::Generator() : Component("Generator")
{
}


Generator::~Generator()
{
}

GEAR_API Generator * Generator::Create()
{
	return new Generator();
}

GEAR_API void Generator::Update(float)
{
	
}

GEAR_API void Generator::DisplayDebug()
{
	
}

GEAR_API void Generator::Init()
{
}

GEAR_API void Generator::Serialize(Json::Value)
{
	
}

GEAR_API void Generator::DeSerialize(Json::Value &a_root, bool a_ignoreDefault)
{
	
}

GEAR_API void Generator::Reset()
{
	
}

GEAR_API void Generator::HandleEvent(Event * a_pEvent)
{
	
}
