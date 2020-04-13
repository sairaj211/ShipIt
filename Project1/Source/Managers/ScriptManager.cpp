#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ScriptManager.h"
#include "EventManager.h"
#include "MemoryManager.h"
#include "../Events/UpdatedScriptEvent.h"

ScriptManager* ScriptManager::mInstance = nullptr;
using namespace std::filesystem;

namespace py = pybind11;

ScriptManager::ScriptManager()
{
}


ScriptManager::~ScriptManager()
{
}

void ScriptManager::Init()
{
	LOG_INFO("ScriptManager Init");
	mFileWrites = std::unordered_map<std::string, file_time_type>();
	py::initialize_interpreter();
	py::module::import("fge");
	py::exec("from fge import *");
	
}

void ScriptManager::Update(float dt)
{
	for (auto kv : mFileWrites) {
		//check each for any file updates
		path _path = path(kv.first);
		file_time_type time = last_write_time(_path);
		if (time > kv.second) {
			//appropriate event firing
			
			UpdatedScriptEvent* uSE = dynamic_cast<UpdatedScriptEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UpdatedScriptEvent"));
			if (nullptr != uSE) {
				uSE->SetUpdate("UPDATE");
				uSE->SetFile(kv.first);
				mFileWrites[kv.first] = time;
				EventManager::GetInstance()->BroadcastEvent(uSE);
			}
			else {
				LOG_WARN("Could not update script, no available events in Memory Manager");
			}
		}
	}
}

void ScriptManager::CleanUp()
{
	py::finalize_interpreter();
	mFileWrites.clear();
}

ScriptManager * ScriptManager::GetInstance()
{
	if (nullptr == ScriptManager::mInstance) {
		ScriptManager::mInstance = new ScriptManager();
	}
	return ScriptManager::mInstance;
}

void ScriptManager::StoreScript(std::string filePath)
{
	if (mFileWrites.find(filePath) == mFileWrites.end()) {
		path _path = path(filePath);
		if (!exists(_path)) {
			LOG_CRITICAL(filePath + " not found");
			return;
		}
		file_time_type time = last_write_time(_path);
		mFileWrites.emplace(filePath, time);
	}
}
