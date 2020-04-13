#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ScriptComponent.h"

#include "Managers/ComponentManager.h"
#include "Managers/ScriptManager.h"
#include "Events/PauseEvent.h"
#include "Events/UpdatedScriptEvent.h"
#include "Events/UpdateLevelEvent.h"
#include "Managers/ResourceManager.h"
#include "Managers/Log.h"
#include "Engine.h"
#include "Resource.h"
#include "Entity.h"
#include "Resources/JsonData.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"

#include <filesystem>

using namespace std::filesystem;


ScriptComponent::ScriptComponent() : Component("ScriptComponent")
{
	mScriptModules =std::unordered_map<std::string,py::module>();
	mIsAlwaysRun = false;
}

ScriptComponent::~ScriptComponent()
{
	current = 0;
}

ScriptComponent * ScriptComponent::Create()
{
	return new ScriptComponent();
}

void ScriptComponent::Update(float deltaTime)
{
	
	if (!mPaused || mIsAlwaysRun) {
		//need to see if the python path needs to 
		//be constantly updated here
		for (auto kv : mScriptModules) {

			//try to run the module's on_update function
			try {
				kv.second.attr("on_update").call(mOwnerID,deltaTime);
			}
			catch (const py::error_already_set& e ) {
				PyErr_PrintEx(0);
				LOG_WARN(e.what());
			}
		}
	}
}

void ScriptComponent::DisplayDebug()
{
// 	const char* items[2];
// 	int i = 0;
// 	for (std::pair<std::string, py::module> element : mScriptModules) {
// 		items[i] = element.first.c_str();
// 		i++;
// 		}
// 
// 	ImGui::ListBox("Scripts name", &current, items, IM_ARRAYSIZE(items), IM_ARRAYSIZE(items));

}

void ScriptComponent::DeSerialize(Json::Value & root, bool a_ignoreDefault)
{
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["ScriptComponent"];

	bool isDifferent = defaultRoot["scripts"].size() != mScriptModules.size();
	if (!isDifferent) {
		for (unsigned int i = 0; i < defaultRoot["scripts"].size(); ++i) {
			if (mScriptModules.end() == mScriptModules.find(defaultRoot["scripts"][i].asString())) {
				isDifferent = true;
				break;
			}
		}
	}

	if (isDifferent || a_ignoreDefault) {
		Json::Value scripts(Json::arrayValue);
		for (auto kv : mScriptModules) {
			scripts.append(kv.first);
		}
		root["scripts"] = scripts;
	}

	if (mIsAlwaysRun != defaultRoot["always-run"].asBool() || a_ignoreDefault)
		root["always-run"] = mIsAlwaysRun;
}
void ScriptComponent::Serialize(Json::Value root)
{
	Json::Value scripts = root["scripts"];
	if (!scripts.isNull()) {
		mScriptModules.clear();
		int size = scripts.size();
		for (int i = 0; i < size; ++i) {
			AddScript(scripts[i].asString(), false);
			ScriptManager::GetInstance()->StoreScript(scripts[i].asString());
		}
	}
	Json::Value run = root["always-run"];
	if (!run.isNull())
		mIsAlwaysRun = run.asBool();
}

void ScriptComponent::HandleEvent(Event * pEvent)
{
	if (pEvent->GetType() == "PauseEvent") 
	{
		PauseEvent* peEvent = static_cast<PauseEvent*>(pEvent);
		if (nullptr != peEvent) {
			mPaused = peEvent->mPause && !mIsAlwaysRun;
		}
	} 
	if (pEvent->GetType() == "UpdatedScriptEvent")
	{
		UpdatedScriptEvent* uEvent = dynamic_cast<UpdatedScriptEvent*>(pEvent);
		if (uEvent->GetUpdate() == "DELETE") {
			if (mScriptModules.find(uEvent->GetFile()) != mScriptModules.end()) 
			{
				mScriptModules.erase(uEvent->GetFile());
			}
		}
		else if (uEvent->GetUpdate() == "UPDATE") {
			if (mScriptModules.find(uEvent->GetFile()) != mScriptModules.end()) 
			{
					mScriptModules[uEvent->GetFile()].reload();
			}
		}
		else if (uEvent->GetUpdate() == "ADD") 
		{
			 //Add module here
			AddScript(uEvent->GetFile(), true);
		}
	}
	if (pEvent->GetType() == "UpdateLevelEvent")
	{
		UpdateLevelEvent* pUpdatedLevelEvent = static_cast<UpdateLevelEvent*>(pEvent);
		for (auto kv : mScriptModules) {

			//try to run the module's on_update function
			try {
				kv.second.attr("on_event")(mOwnerID, pUpdatedLevelEvent);
			}
			catch (const py::error_already_set& e) {
				PyErr_PrintEx(0);
				LOG_WARN(e.what());
			}
		}
	}
	else 
	{
			//need to see if the python path needs to 
			//be constantly updated here
			for (auto kv : mScriptModules) {

				//try to run the module's on_update function
				try {
					kv.second.attr("on_event")(mOwnerID, pEvent);
				}
				catch (const py::error_already_set& e) {
					PyErr_PrintEx(0);
					LOG_WARN(e.what());
				}
			}
		
	}
}

int ScriptComponent::GetId()
{
	return mOwnerID;
}

void ScriptComponent::Init()
{
	for (auto kv : mScriptModules) {
		
		try {
			kv.second.attr("on_init").call(mOwnerID);
		}
		catch (const py::error_already_set& e) {
			PyErr_PrintEx(0);
			LOG_ERROR("SCRIPT INIT ERROR {}", e.what());
		}
		
	}
}

GEAR_API void ScriptComponent::Reset()
{
	mScriptModules.clear();
	Component::Reset();
}

void ScriptComponent::AddScript(std::string _path, bool init)
{
	
	std::vector<std::string> tokens;
	std::istringstream f(_path); std::string s;
	while (getline(f, s, '\\')) { tokens.push_back(s); }

	//build the path to the file
	std::string path_to_file = std::string();
	for (int i = 0; i < tokens.size() - 1; ++i) {
		path_to_file += tokens[i] + "\\";
	}
	py::module sys = py::module::import("sys");
	py::object count = sys.attr("path").attr("count")(path_to_file.c_str());
	double c = PyFloat_AsDouble(count.ptr());
	if(c == 0)
		sys.attr("path").attr("append")(path_to_file.c_str());

	//grab the module name (the file name without .py)
	std::string python_file = tokens[tokens.size() - 1];
	std::istringstream p(python_file);

	std::vector<std::string> py_tokens;
	while (getline(p, s, '.')) { py_tokens.push_back(s); }

	std::string module_name = py_tokens[0];
	
	//try to load the module
	try {
		py::module mod = py::module::import(module_name.c_str());
		if (mScriptModules.find(_path) == mScriptModules.end()) 
		{
			mScriptModules.emplace(_path, mod);
		} 
		else 
		{
			mScriptModules[_path].reload();
		}	
		if (init) {
			mScriptModules[_path].attr("on_init")(mOwnerID);
		}
	}
	catch (const py::error_already_set& e) {
		LOG_CRITICAL("SCRIPT LOAD ERROR ");
		LOG_ERROR(e.what());
		PyErr_PrintEx(0);
		
		
	}

}

GEAR_API void ScriptComponent::Clone(int objID)
{
	ScriptComponent *rhs = static_cast<ScriptComponent*>(COMPONENT_MANAGER->GetComponent<ScriptComponent>(objID, "ScriptComponent"));

	mScriptModules = rhs->mScriptModules;
	mIsAlwaysRun = rhs->mIsAlwaysRun;
}
