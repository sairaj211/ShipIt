#pragma once

std::vector<Component*> GetComponentsForEntity(int id);

std::vector<Component*> GetComponents(std::string name);

template<typename T>
T* GetComponent(int id, std::string name, bool ignoreEnabled = false) {
	return ComponentManager::GetInstance()->GetComponent<T>(id, name, ignoreEnabled);
}

template<typename T>
T* GetRuntimeComponent(int id, std::string name) {
	return ComponentManager::GetInstance()->GetRuntimeComponent<T>(id, name);
}

ContactEvent* GetContactEvent();

ResetEntityEvent* GetResetEvent();

RecipeMatchEvent* GetRecipeMatchEvent();

PlayerPickupEvent* GetPlayerPickupEvent();

PlayerDropEvent* GetPlayerDropEvent();

ContainerInputEvent* GetContainerInputEvent();

ContainerOutputEvent* GetContainerOutputEvent();

ContainerMatchEvent* GetContainerMatchEvent();

RequestFilledEvent* GetRequestFilledEvent();

RequestMadeEvent* GetRequestMadeEvent();

UpdateLevelEvent* GetUpdateLevelEvent();

GeneratorOutputEvent* GetGeneratorOutputEvent();

GeneratorInputEvent* GetGeneratorInputEvent();

InputEvent* GetInputEvent();

SetCellEvent* GetSetCellEvent();

CellUpdateEvent* GetCellUpdateEvent();

CellEjectEvent* GetCellEjectEvent();

CreateGameEvent* GetCreateGameEvent();

InitLevelEvent* GetInitLevelEvent();

ResetGridEvent* GetResetGridEvent();

HighlightEvent* GetHighlightEvent();

ChangeAnimationStateEvent* GetChangeAnimationStateEvent();

PauseEvent* GetPauseEvent();

void SendEvent(Event* a_Event, int a_id);

void BroadcastEvent(Event* a_Event);

void SendTimedEvent(Event* a_Event);

std::vector<int> GetEntities(std::string type, bool ignoreEnabled = false);

bool HasParent(int id);

std::vector<int> GetChildren(int id, std::string type);

std::string GetType(int id);

bool IsEnabled(int id);

void DeleteEntity(int id);

int CreateEntity(std::string dataFile, int parentID = -1);

void ChangeScene(std::string sceneName);

void ClearRuntimeLists();

std::string GetMaterialName(std::string a_prefab);

std::vector<float> lerp3(std::vector<float> vec1, std::vector<float> vec2, float t);