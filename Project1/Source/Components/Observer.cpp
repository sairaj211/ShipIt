#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Observer.h"


Observer::Observer():Component("Observer")
{
	mValues = std::unordered_map<std::string, float>();
}

Observer::~Observer()
{
	mValues.clear();
}

GEAR_API Observer * Observer::Create()
{
	return new Observer();
}

GEAR_API void Observer::Update(float)
{
	//should be handled in script
}

GEAR_API void Observer::DisplayDebug()
{
	for (auto kv : mValues) {
		ImGui::InputFloat(kv.first.c_str(), &kv.second);
	}
}

GEAR_API void Observer::Init()
{
	//should be handled in script
}

GEAR_API void Observer::Serialize(Json::Value){}

GEAR_API void Observer::DeSerialize(Json::Value &a_root, bool a_ignoreDefault){}

GEAR_API void Observer::HandleEvent(Event * a_pEvent)
{
	//should be handled in script
}

GEAR_API void Observer::Reset()
{
	mValues.clear();
}

GEAR_API void Observer::SetValue(std::string a_key, float a_value)
{
	if (mValues.find(a_key) == mValues.end()) {
		mValues.emplace(a_key, a_value);
	}
	else 
	{
		mValues[a_key] = a_value;
	}
}

float Observer::GetValue(std::string a_key)
{
	if (mValues.find(a_key) != mValues.end()) {
		return mValues[a_key];
	}
	return NAN;
}
