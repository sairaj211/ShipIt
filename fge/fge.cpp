#include "pch.h"

#include "Engine.h"

#include "Managers/Log.h"
#include "Managers/InputManager.h"
#include "Managers/CameraManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/AudioManager.h"
#include "Managers/ParticleManager.h"
#include "Managers/EntityManager.h"
#include "Managers/LocalizationManager.h"
#include "Managers/Renderer.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/EnemyStateManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/SceneManager.h"
#include "Editor/LevelEditor.h"

#include "Physics/PhysicsManager.h"
#include "Physics/Shape.h"

#include "Input/Mouse.h"

#include "Random/Noise.h"
#include "Random/RandomNumberGenerator.h"

#include "Editor/RayCast.h"

#include "Resources/JsonData.h"

#include "Entity.h"

#include "Component.h"
#include "Components/Animator.h"
#include "Components/Collider.h"
#include "Components/ModelComponent.h"
#include "Components/Transform.h"
#include "Components/Controller.h"
#include "Components/ScriptComponent.h"
#include "Components/Player.h"
#include "Components/ParticlesComponent.h"
#include "Components/Audio.h"
#include "Components/ButtonComponent.h"
#include "Components/Camera.h"
#include "Components/AreaSearch.h"
#include "Components/Carrier.h"
#include "Components/Observer.h"
#include "Components/Processor.h"
#include "Components/Enemy.h"
#include "Components/Bullet.h"
#include "Components/Wall.h"
#include "Components/Grid.h"
#include "Components/GridWaypoint.h"
#include "Components/Block.h"
#include "Components/AffineSequence.h"
#include "Components/LevelButton.h"

#include "Resources/Texture.h"
#include "Resources/Material.h"

#include "Event.h"
#include "Events/ContactEvent.h"
#include "Events/AttackEvent.h"
#include "Events/ResetEntityEvent.h"
#include "Events/RecipeMatchEvent.h"
#include "Events/PlayerPickupEvent.h"
#include "Events/PlayerDropEvent.h"
#include "Events/ContainerOutputEvent.h"
#include "Events/ContainerInputEvent.h"
#include "Events/ContainerMatchEvent.h"
#include "Events/RequestFilledEvent.h"
#include "Events/RequestMadeEvent.h"
#include "Events/GeneratorOutputEvent.h"
#include "Events/GeneratorInputEvent.h"
#include "Events/InputEvent.h"
#include "Events/SetCellEvent.h"
#include "Events/CellUpdateEvent.h"
#include "Events/CellEjectEvent.h"
#include "Events/CreateGameEvent.h"
#include "Events/InitLevelEvent.h"
#include "Events/UpdateLevelEvent.h"
#include "Events/HighlightEvent.h"
#include "Events/ChangeAnimationStateEvent.h"
#include "Events/LevelCompleteEvent.h"
#include "Events/ResetGridEvent.h"

#include "Utility.h"

#include "fge.h"

namespace py = pybind11;

std::vector<Component*> GetComponentsForEntity(int id) {
	return ComponentManager::GetInstance()->GetComponents(id);
}

std::vector<Component*> GetComponents(std::string name) {
	return ComponentManager::GetInstance()->GetComponents(name);
}

ContactEvent* GetContactEvent() {
	return static_cast<ContactEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ContactEvent"));
}

ResetEntityEvent* GetResetEvent() {
	return static_cast<ResetEntityEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ResetEntityEvent"));
}

RecipeMatchEvent* GetRecipeMatchEvent() {
	return static_cast<RecipeMatchEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("RecipeMatchEvent"));
}

PlayerPickupEvent* GetPlayerPickupEvent() {
	return static_cast<PlayerPickupEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("PlayerPickupEvent"));
}

PlayerDropEvent* GetPlayerDropEvent() {
	return static_cast<PlayerDropEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("PlayerDropEvent"));
}

ContainerInputEvent* GetContainerInputEvent() {
	return static_cast<ContainerInputEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ContainerInputEvent"));
}

ContainerOutputEvent* GetContainerOutputEvent() {
	return static_cast<ContainerOutputEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ContainerOutputEvent"));
}

ContainerMatchEvent* GetContainerMatchEvent() {
	return static_cast<ContainerMatchEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ContainerMatchEvent"));
}

RequestFilledEvent* GetRequestFilledEvent() {
	return static_cast<RequestFilledEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("RequestFilledEvent"));
}

RequestMadeEvent* GetRequestMadeEvent() {
	return static_cast<RequestMadeEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("RequestMadeEvent"));
}

UpdateLevelEvent* GetUpdateLevelEvent() {
	return static_cast<UpdateLevelEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UpdateLevelEvent"));
}

GeneratorOutputEvent* GetGeneratorOutputEvent() {
	return static_cast<GeneratorOutputEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("GeneratorOutputEvent"));
}

GeneratorInputEvent* GetGeneratorInputEvent() {
	return static_cast<GeneratorInputEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("GeneratorInputEvent"));
}

InputEvent* GetInputEvent() {
	return static_cast<InputEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("InputEvent"));
}

SetCellEvent* GetSetCellEvent() {
	return static_cast<SetCellEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("SetCellEvent"));
}

CellUpdateEvent* GetCellUpdateEvent() {
	return static_cast<CellUpdateEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("CellUpdateEvent"));
}

CellEjectEvent* GetCellEjectEvent() {
	return static_cast<CellEjectEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("CellEjectEvent"));
}

CreateGameEvent* GetCreateGameEvent() {
	return static_cast<CreateGameEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("CreateGameEvent"));
}

InitLevelEvent* GetInitLevelEvent() {
	return static_cast<InitLevelEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("InitLevelEvent"));
}

LevelCompleteEvent* GetLevelCompleteEvent() {
	return static_cast<LevelCompleteEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("LevelCompleteEvent"));
}

HighlightEvent* GetHighlightEvent() {
	return static_cast<HighlightEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("HighlightEvent"));
}

ChangeAnimationStateEvent* GetChangeAnimationStateEvent() {
	return static_cast<ChangeAnimationStateEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ChangeAnimationStateEvent"));
}

PauseEvent* GetPauseEvent() {
	return static_cast<PauseEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("PauseEvent"));
}

ResetGridEvent* GetResetGridEvent() {
	return static_cast<ResetGridEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ResetGridEvent"));
}

void SendEvent(Event* a_Event, int a_id) {
	EventManager::GetInstance()->SendEvent(a_Event, a_id);
}

void BroadcastEvent(Event* a_Event) {
	EventManager::GetInstance()->BroadcastEvent(a_Event);
}

void SendTimedEvent(Event* a_Event) {
	EventManager::GetInstance()->AddTimedEvent(a_Event);
}

std::vector<int> GetEntities(std::string type, bool ignoreEnabled) {
	std::vector<Entity*> typeEntities = ENTITY_MANAGER->GetEntities(type, ignoreEnabled);
	std::vector<int> typeEntitiesIDs;
	for (Entity* pEntity : typeEntities)
		typeEntitiesIDs.push_back(pEntity->GetID());
	return typeEntitiesIDs;
}

static AttackEvent* GetAttackEvent() {
	return static_cast<AttackEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("AttackEvent"));
}

bool HasParent(int id) {
	int parentID = ENTITY_MANAGER->GetEntity(id, true)->GetParentID();
	return ENTITY_MANAGER->GetEntity(parentID) != nullptr;
}

std::vector<int> GetChildren(int id, std::string type) {
	return ENTITY_MANAGER->GetEntity(id)->GetChildIDs(type);
}

std::string GetType(int id) {
	Entity* pEntity = ENTITY_MANAGER->GetEntity(id);
	std::string type = "";
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(id);

	if (pEntity)
		type = pEntity->GetType();

	return type;
}

bool IsEnabled(int id) {
	return ENTITY_MANAGER->GetEntity(id, true)->IsEnabled();
}

void DeleteEntity(int id) {
	ENTITY_MANAGER->DeleteEntity(id);
}

static void SetParentID(int id, int otherID) {
	ENTITY_MANAGER->GetEntity(id, true)->SetParentID(otherID);
}

static int GetParentID(int id) {
	int parentID = ENTITY_MANAGER->GetEntity(id, true)->GetParentID();
	return parentID;
}

std::string GetName(int id) {
	return ENTITY_MANAGER->GetEntity(id)->GetName();
}


int CreateEntity(std::string dataFile, int parentID ){
	Entity* entity = ENTITY_MANAGER->CreateRuntimeEntity(dataFile,parentID);
	return entity->GetID();
}

void ChangeScene(std::string sceneName) {
	SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
	SCENE_MANAGER->PushScene(sceneName);
	OBJECT_FACTORY->LoadLevel(sceneName);
}

void ClearRuntimeLists() {
	ENTITY_MANAGER->ClearRuntimeLists();
	COMPONENT_MANAGER->ClearRuntimeLists();
	EventManager::GetInstance()->Clean();
}

std::string GetMaterialName(std::string a_prefab) {
	Json::Value prefabData = RESOURCE_MANAGER->LoadJsonData(a_prefab, ".\\Resources\\Prefab\\")->mRoot;
	Json::Value material = prefabData["ModelComponent"]["Material"];
	std::string materialName = "Default";
	if (!material.isNull())
		materialName = material.asString();

	return materialName;
}

vector<float> ScaleVec(float scale, std::vector<float> vec) {
	std::vector<float>result;
	for (float v : vec) 
		result.push_back(v*scale);
	return result;
}

vector<float> AddVec(std::vector<float> vec1, std::vector<float> vec2) {
	std::vector<float>result;
	for (int i = 0; i < vec1.size();++i) 
		result.push_back(vec1[i]+vec2[i] );
	return result;
}


vector<float> SubVec(std::vector<float> vec1, std::vector<float> vec2) {
	std::vector<float>result;
	for (int i = 0; i < vec1.size(); ++i) 
		result.push_back(vec1[i] - vec2[i]);
	return result;
}


float DotVec(std::vector<float> vec1, std::vector<float> vec2) {
	float result = 0;
	for (int i = 0; i < vec1.size(); ++i)
		result += (vec1[i] * vec2[i]);
	return result;
}

std::vector<float> lerp3(std::vector<float> vec1, std::vector<float> vec2, float t) {
	std::vector<float> result;
	glm::vec3 resultVec = glm::lerp(glm::vec3(vec1[0], vec1[1], vec1[2]), glm::vec3(vec2[0], vec2[1], vec2[2]), t);
	for (int i = 0; i < 3; ++i) {
		result.push_back(resultVec[i]);
	}
	return result;
}

std::vector<float> rotate(std::vector<float> vector, Transform* a_pTransform) {
	glm::vec4 resultVec = glm::vec4(vector[0], vector[1], vector[2], 0.0f);
	resultVec = a_pTransform->GetRotationMatrix() * resultVec;
	std::vector<float> result;
	for (int i = 0; i < 3; ++i) {
		result.push_back(resultVec[i]);
	}
	return result;
}

/*!
 * Binding c++ methods to fge 
 * 
 * \param fge the name of the module
 * \param m the module
 * \return Builds the python pyd file binding python to c++ code
 */
PYBIND11_MODULE(fge, m)
{
	m.doc() = "pybind11 Fifth Gear Engine module"; // optional module docstring
	
	// Utility
	m.def("ToWString", &ToWString);
	m.def("DecodeResource", &DecodeResource);
	m.def("DecodeGame", &DecodeGame);

	//vector stuff
	m.def("Dot", &DotVec);
	m.def("Add", &AddVec);
	m.def("Sub", &SubVec);
	m.def("Scale", &ScaleVec);
	m.def("rotate", &rotate);
	m.def("lerp3", &lerp3);

	//Component Getters
	m.def("GetComponentsForEntity", &GetComponentsForEntity, py::return_value_policy::reference);
	m.def("GetComponents", &GetComponents, py::return_value_policy::reference);
	
	m.def("GetTransform", GetComponent<Transform>, py::arg("id"), py::arg("name") = "Transform", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetCollider", GetComponent<Collider>, py::arg("id"), py::arg("name") = "Collider", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetController", GetComponent<Controller>, py::arg("id"), py::arg("name") = "Controller", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetBody", GetComponent<Body>, py::arg("id"), py::arg("name") = "Body", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetAnimator", GetComponent<Animator>, py::arg("id"), py::arg("name") = "Animator", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetPlayer", GetComponent<Player>, py::arg("id"), py::arg("name") = "Player", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetAudio", GetComponent<Audio>, py::arg("id"), py::arg("name") = "Audio", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetButton", GetComponent<ButtonComponent>, py::arg("id"), py::arg("name") = "ButtonComponent", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetParticles", GetComponent<ParticlesComponent>, py::arg("id"), py::arg("name") = "ParticlesComponent", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetModel", GetComponent<ModelComponent>, py::arg("id"), py::arg("name") = "ModelComponent", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetCamera", GetComponent<Camera>, py::arg("id"), py::arg("name") = "Camera", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetAreaSearch", GetComponent<AreaSearch>, py::arg("id"), py::arg("name") = "AreaSearch", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetCarrier", GetComponent<Carrier>, py::arg("id"), py::arg("name") = "Carrier", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetProcessor", GetComponent<Processor>, py::arg("id"), py::arg("name") = "Processor", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetObserver", GetComponent<Observer>, py::arg("id"), py::arg("name") = "Observer", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetBullet", GetComponent<Bullet>, py::arg("id"), py::arg("name") = "BulletComp", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetEnemy", GetComponent<Enemy>, py::arg("id"), py::arg("name") = "Enemy", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetWall", GetComponent<Wall>, py::arg("id"), py::arg("name") = "Wall", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetGrid", GetComponent<Grid>, py::arg("id"), py::arg("name") = "Grid", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetGridWaypoint", GetComponent<GridWaypoint>, py::arg("id"), py::arg("name") = "GridWaypoint", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetAffineSequence", GetComponent<AffineSequence>, py::arg("id"), py::arg("name") = "AffineSequence", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetBlock", GetComponent<Block>, py::arg("id"), py::arg("name") = "Block", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);
	m.def("GetLevelButton", GetComponent<LevelButton>, py::arg("id"), py::arg("name") = "LevelButton", py::arg("ignoreEnabled") = true, py::return_value_policy::reference);


	m.def("GetRuntimeCollider", GetRuntimeComponent<Collider>, py::arg("id"), py::arg("name") = "Collider", py::return_value_policy::reference);
	m.def("GetRuntimeTransform", GetRuntimeComponent<Transform>, py::arg("id"), py::arg("name") = "Transform", py::return_value_policy::reference);
	m.def("GetRuntimeBody", GetRuntimeComponent<Body>, py::arg("id"), py::arg("name") = "Body", py::return_value_policy::reference);
	m.def("GetRuntimeModel", GetRuntimeComponent<ModelComponent>, py::arg("id"), py::arg("name") = "ModelComponent", py::return_value_policy::reference);
	m.def("GetRuntimeEnemy", GetRuntimeComponent<Enemy>, py::arg("id"), py::arg("name") = "Enemy", py::return_value_policy::reference);

	//Event bindings
	m.def("GetContactEvent", &GetContactEvent, py::return_value_policy::reference);
	m.def("GetResetEvent", &GetResetEvent, py::return_value_policy::reference);
	m.def("GetAttackEvent",&GetAttackEvent, py::return_value_policy::reference);
	m.def("GetRecipeMatchEvent", &GetRecipeMatchEvent, py::return_value_policy::reference);
	m.def("GetPlayerPickupEvent", &GetPlayerPickupEvent, py::return_value_policy::reference);
	m.def("GetPlayerDropEvent", &GetPlayerDropEvent, py::return_value_policy::reference);
	m.def("GetContainerInputEvent", &GetContainerInputEvent, py::return_value_policy::reference);
	m.def("GetContainerOutputEvent", &GetContainerOutputEvent, py::return_value_policy::reference);
	m.def("GetContainerMatchEvent", &GetContainerMatchEvent, py::return_value_policy::reference);
	m.def("GetRequestFilledEvent", &GetRequestFilledEvent, py::return_value_policy::reference);
	m.def("GetRequestMadeEvent", &GetRequestMadeEvent, py::return_value_policy::reference);
	m.def("GetGeneratorOutputEvent", &GetGeneratorOutputEvent, py::return_value_policy::reference);
	m.def("GetGeneratorInputEvent", &GetGeneratorInputEvent, py::return_value_policy::reference);
	m.def("GetInputEvent", &GetInputEvent, py::return_value_policy::reference);
	m.def("GetSetCellEvent", &GetSetCellEvent, py::return_value_policy::reference);
	m.def("GetCellUpdateEvent", &GetCellUpdateEvent, py::return_value_policy::reference);
	m.def("GetCellEjectEvent", &GetCellEjectEvent, py::return_value_policy::reference);
	m.def("GetCreateGameEvent", &GetCreateGameEvent, py::return_value_policy::reference);
	m.def("GetInitLevelEvent", &GetInitLevelEvent, py::return_value_policy::reference);
	m.def("GetUpdateLevelEvent", &GetUpdateLevelEvent, py::return_value_policy::reference);
	m.def("GetHighlightEvent", &GetHighlightEvent, py::return_value_policy::reference);
	m.def("GetChangeAnimationStateEvent", &GetChangeAnimationStateEvent, py::return_value_policy::reference);
	m.def("GetPauseEvent", &GetPauseEvent, py::return_value_policy::reference);
	m.def("GetLevelCompleteEvent", &GetLevelCompleteEvent, py::return_value_policy::reference);
	m.def("GetResetGridEvent", &GetResetGridEvent, py::return_value_policy::reference);


	m.def("BroadcastEvent", &BroadcastEvent);
	m.def("SendEvent", &SendEvent);
	m.def("SendTimedEvent", &SendTimedEvent);


	// Other
	m.def("GetEntities", &GetEntities, py::arg("type"), py::arg("ignoreEnabled") = false);
	m.def("HasParent", &HasParent);
	m.def("GetParentID", &GetParentID);
	m.def("SetParentID", &SetParentID);
	m.def("GetChildren", &GetChildren);
	m.def("GetType", &GetType);
	m.def("IsEnabled", &IsEnabled);
	m.def("GetName", &GetName);
	m.def("DeleteEntity", &DeleteEntity);
	m.def("CreateEntity", &CreateEntity, py::arg("dataFile"), py::arg("parentID") = -1);
	m.def("ChangeScene", &ChangeScene);
	m.def("ClearRuntimeLists", &ClearRuntimeLists);
	m.def("GetMaterial", &GetMaterialName);

	//Components
	py::class_<Component>(m, "Component")
		.def("GetType", &Component::GetType)
		.def("GetOwnerID", &Component::GetOwnerID);

	py::class_<Animator, Component>(m, "Animator")
		.def_readwrite("mPause", &Animator::mPause)
		.def_readwrite("mSpeedModifier", &Animator::mSpeedModifier)
		.def("ChangeState", &Animator::ChangeState)
		.def("ChangeAffineState", &Animator::ChangeAffineState)
		.def("ChangeSkeletalState", &Animator::ChangeSkeletalState)
		.def("GetSkellState", &Animator::GetSkellState)
		.def("GetOwnerID", &Animator::GetOwnerID);;

	py::class_<Audio, Component>(m, "Audio")
		.def_readonly("mChannelID",&Audio::mChannelID)
		.def("Play", (void (Audio::*)()) &Audio::Play)
		.def("Play", (void (Audio::*)(int)) &Audio::Play)
		.def("Load", &Audio::Load, py::arg("sound"), py::arg("isLoop") = false, py::arg("is3D") = false)
		.def("PlayLoop", &Audio::PlayLoop)
		.def("Stop", &Audio::Stop)
		.def("Volume", &Audio::Volume)
		.def("Mute", &Audio::Mute)
		.def("FadeOut", &Audio::FadeOut)
		.def("FadeIn", &Audio::FadeIn)
		.def("TogglePause", &Audio::TogglePause);

	py::class_<Body, Component>(m, "Body")
		.def_readwrite("mConstrainX", &Body::mConstrainX)
		.def_readwrite("mConstrainY", &Body::mConstrainY)
		.def_readwrite("mConstrainZ", &Body::mConstrainZ)
		.def_readwrite("mUseGravity", &Body::mUseGravity)
		.def("Init", &Body::Init)
		.def("GetVelocity", &Body::GetVelocity)
		.def("SetVelocity", &Body::SetVelocity);


	py::class_<ButtonComponent, Component>(m, "Button")
		.def_readonly("mPlayerId", &ButtonComponent::mPlayerId)
		.def("IsPressed", &ButtonComponent::IsPressed)
		.def("IsEntered", &ButtonComponent::IsTriggered)
		.def("IsReleased", &ButtonComponent::IsReleased);

	py::class_<Camera, Component>(m, "Camera")
		.def("SetIsDefault", &Camera::SetIsDefault)
		.def("IsDefault", &Camera::IsDefault)
		.def("GetFront", &Camera::GetFront)
		.def("GetUp", &Camera::GetUp)
		.def("GetRight", &Camera::GetRight)
		.def("SetNearPlane", &Camera::SetNearPlane)
		.def("GetNearPlane", &Camera::GetNearPlane)
		.def("SetFarPlane", &Camera::SetFarPlane)
		.def("GetFarPlane", &Camera::GetFarPlane)
		.def("SetFieldOfView", &Camera::SetFieldOfView)
		.def("GetFieldOfView", &Camera::GetFieldOfView)
		.def("GetAspectRatio", &Camera::GetAspectRatio);

	py::class_<Collider, Component>(m, "Collider")
		.def_readwrite("mIsTrigger", &Collider::mIsTrigger)
		.def("mIsCollided", &Collider::IsCollided)
		.def("mOwnerID", &Collider::GetOwner);

	py::class_<Controller, Component>(m, "Controller")
		.def("IsPressed", &Controller::IsPressed)
		.def("IsTriggered", &Controller::IsTriggered)
		.def("IsReleased", &Controller::IsReleased)
		.def("GetPlayer",&Controller::GetControllerId);

	py::class_<ModelComponent, Component>(m, "Model")
		.def("MakeRainbow", &ModelComponent::MakeRainbow)
		.def("Highlight", &ModelComponent::Highlight)
		.def_readwrite("mpMaterial", &ModelComponent::mpMaterial);

	py::class_<LevelButton, Component>(m, "LevelButton")
		.def_readwrite("LevelID", &LevelButton::mLevelID)
		.def_readwrite("WorldID", &LevelButton::mWorldID)
		.def_readwrite("LevelName", &LevelButton::mLevelName)
		.def_readwrite("isUnlocked", &LevelButton::mIsUnlocked)
		.def_readwrite("IN", &LevelButton::in)
		.def_readwrite("UnlockScore", &LevelButton::mUnlockScore)
		.def_readwrite("HighScore", &LevelButton::mHighScore);


	py::class_<ParticlesComponent, Component>(m, "ParticlesComponent")
		.def("GetOwnerID", &ParticlesComponent::GetOwnerID)
		.def("SetRunning", &ParticlesComponent::SetRunning)
		.def_readwrite("mOffset", &ParticlesComponent::mOffset)
		.def_readwrite("mRange", &ParticlesComponent::mRange)
		.def_readwrite("mVelocity", &ParticlesComponent::mVelocity)
		.def_readwrite("mRotation", &ParticlesComponent::mRotation)
		.def_readwrite("mScale", &ParticlesComponent::mScale)
		.def_readwrite("mDirection", &ParticlesComponent::mDirection)
		.def_readwrite("mColor", &ParticlesComponent::mColor)
		.def_readwrite("mLife", &ParticlesComponent::mLife)
		.def_readwrite("mNumOfParticles", &ParticlesComponent::mNumOfParticles)
		.def_readwrite("mActiveParticles", &ParticlesComponent::mActiveParticles)
		.def_readwrite("mMinParticles", &ParticlesComponent::mMinParticles)
		.def_readwrite("m_strMaterialName", &ParticlesComponent::m_strMaterialName)
		.def_readwrite("m_uLastUnusedParticle", &ParticlesComponent::m_uLastUnusedParticle);

	py::class_<Player, Component>(m, "Player")
		.def_readonly("mAngle", &Player::mAngle)
		.def_readwrite("mAttack", &Player::mAttack)
		.def_readwrite("mKilled", &Player::mKilled)
		.def_readwrite("mPowerUps", &Player::mPowerUps)
		.def_readonly("mHitTime",&Player::mHitTime)
		.def_readonly("mRechargeTime", &Player::mRechargeTime)
		.def("MakeAttack", &Player::MakeAttack)
		.def("GetOwnerID", &Player::GetOwnerID)
		.def("Move", &Player::Move, py::arg("x"), py::arg("y"), py::arg("sprint") = false)
		.def("Rotate", &Player::Rotate)
		.def("Jump", &Player::Jump)
		.def("IsGrounded", &Player::GetIsGrounded)
		.def("SetMaxVelocity", &Player::SetMaxVelocity);

	py::class_<ScriptComponent, Component>(m, "ScriptComponent")
		.def("GetSelf", &ScriptComponent::GetSelf);

	py::class_<Observer, Component>(m, "Observer")
		.def("GetValue", &Observer::GetValue)
		.def("SetValue", &Observer::SetValue);

	py::class_<Transform, Component>(m, "Transform")
		.def("GetLocalScale", &Transform::GetLocalTupleScale)
		.def("GetLocalPosition", &Transform::GetLocalTuplePosition)
		.def("GetPosition",&Transform::GetTuplePosition)
		.def("GetScale",&Transform::GetTupleScale)
		.def("SetLocalScale",&Transform::SetLocalScaleValues)
		.def("SetLocalPosition",&Transform::SetLocalPositionValues)
		.def("SetScale", &Transform::SetScaleValues)
		.def("SetPosition", &Transform::SetPositionValues)
		.def("SetRotation", &Transform::SetRotationAngleValues)
		.def("GetRotation", &Transform::GetRotationAngleValues);

	py::class_<AreaSearch, Component>(m, "AreaSearch")
		.def("GetFound", (std::vector<int> (AreaSearch::*)())&AreaSearch::GetFound)
		.def("GetFound", (std::vector<int> (AreaSearch::*)(std::string))&AreaSearch::GetFound);

	py::class_<Carrier, Component>(m, "Carrier")
		.def("AddItem", &Carrier::AddItem)
		.def("RemoveItem", &Carrier::RemoveItem)
		.def("RemoveItems", &Carrier::RemoveItems)
		.def("Void", &Carrier::Void)
		.def("IsItem", &Carrier::IsItem)
		.def("GetItems", (std::vector<int> (Carrier::*)())&Carrier::GetItems)
		.def("GetItems", (std::vector<int> (Carrier::*)(std::string))&Carrier::GetItems);

	py::class_<Processor, Component>(m, "Processor")
		.def("GetMaxInput", &Processor::GetMaxInput)
		.def("GetRecipes", &Processor::GetRecipes)
		.def("IsIdle", &Processor::IsIdle)
		.def("IsWorking", &Processor::IsWorking)
		.def("IsDone", &Processor::IsDone)
		.def("IsPaused", &Processor::IsPaused)
		.def("Start", &Processor::Start)
		.def("Stop", &Processor::Stop)
		.def("Pause", &Processor::Pause)
		.def("Resume", &Processor::Resume)
		.def("GetMaxProcessingTime", &Processor::GetMaxProcessingTime)
		.def("GetCurrentProcessingTime", &Processor::GetCurrentProcessingTime)
		.def("SetCurrentTime", &Processor::SetCurrentTime);

	py::class_<Bullet, Component>(m, "BulletComp")
		.def("GetOwner", &Bullet::GetOwner);

	py::class_<Enemy, Component>(m, "Enemy")
		.def_readwrite("mHealth",&Enemy::mHealth)
		.def("Destroy", &Enemy::Destroy);

	py::class_<Wall, Component>(m, "Wall")
		.def_readwrite("mHealth", &Wall::mHealth)
		.def_readwrite("mKills", &Wall::mTotalKills)
		.def_readonly("mType", &Wall::mType)
		.def("ChangeColor",(void (Wall::*)(std::string)) &Wall::ChangeColor)
		.def("Destroy", &Wall::Destroy);

	py::class_<Grid, Component>(m, "Grid")
		.def("GetRecipe", &Grid::GetRecipe)
		.def("GetCurrentRecipe", &Grid::GetCurrentRecipe);

	py::class_<GridWaypoint, Component>(m, "GridWaypoint")
		.def("GetMaxDuration", &GridWaypoint::GetMaxDuration)
		.def("GetMinDuration", &GridWaypoint::GetMinDuration)
		.def("GetOrder", &GridWaypoint::GetOrder);
	
	py::class_<AffineSequence, Component>(m, "AffineSequence")
		.def("PlayCurrent", &AffineSequence::PlayCurrent)
		.def("GetDuration", &AffineSequence::GetDuration)
		.def("GetTimer", &AffineSequence::GetTimer)
		.def("GetAnimation", &AffineSequence::GetAnimation)
		.def("ResumeTimer", &AffineSequence::ResumeTimer)
		.def("PauseTimer", &AffineSequence::PauseTimer)
		.def("ResetTimer", &AffineSequence::ResetTimer);

	py::class_<Block, Component>(m, "Block")
		.def("CanInput", &Block::CanInput);

	//Other
	py::class_<Texture>(m, "Texture")
		.def_readonly("mName",&Texture::mName);
	py::class_<Material>(m, "Material")
		.def_readonly("mpTexture", &Material::mpDiffuseTexture)
		.def("GetDiffuseColor", &Material::GetTupleDiffuseColor)
		.def_readonly("mName", &Material::mName);
	py::class_<Shape>(m, "Shape");

	//Noise
	py::class_<Noise, std::unique_ptr<Noise, py::nodelete>>(m, "Noise")
		.def_static("GetInstance", &Noise::GetInstance)
		.def("Noise", &Noise::PerlinNoise);

	//Managers
	py::class_<AudioManager, std::unique_ptr<AudioManager, py::nodelete>>(m, "AudioManager")
		.def_static("GetInstance", &AudioManager::GetInstance)
		.def("Play", (int (AudioManager::*)(std::string, glm::vec3, bool)) &AudioManager::Play)
		.def("Play", (int (AudioManager::*)(std::string, int, glm::vec3, bool)) &AudioManager::Play)
		.def("PlaySFX", &AudioManager::PlaySFX)
		.def("Load", &AudioManager::Load)
		/*.def("Stream", &AudioManager::Stream)*/
		.def("LoadLoop", &AudioManager::LoadLoop)
		.def("Mute", &AudioManager::Mute)
		.def("Pause", &AudioManager::Pause)
		.def("Resume", &AudioManager::Resume)
		.def("PauseAll", &AudioManager::PauseAll)
		.def("UnPauseAll", &AudioManager::UnPauseAll)
		.def("StopAll", &AudioManager::StopAll);

	py::class_ <ResourceManager, std::unique_ptr<ResourceManager, py::nodelete>>(m, "ResourceManager")
		.def_static("GetInstance", &ResourceManager::GetInstance)
		.def("GenerateTexture",&ResourceManager::GenerateTexture, py::return_value_policy::reference)
		.def("LoadMaterial", &ResourceManager::LoadMaterial, py::return_value_policy::reference);

	py::class_<CollisionManager>(m, "CollisionManager")
		.def("CheckContact", &CollisionManager::CheckContact)
		.def("RetrieveFirstContact", (Collider* (CollisionManager::*)(Collider*, std::string, ContactType))&CollisionManager::RetrieveFirstContact, py::return_value_policy::reference);

	py::class_<ComponentManager, std::unique_ptr<ComponentManager, py::nodelete>>(m, "ComponentManager")
		.def_static("GetInstance", &ComponentManager::GetInstance)
		.def("GetComponents", 
			(std::vector<Component*> (ComponentManager::*)(std::string, bool)) &ComponentManager::GetComponents
			, py::return_value_policy::reference, py::arg("a_type"), py::arg("a_ignoreEnabled") = false);

	py::class_<Renderer, std::unique_ptr<Renderer, py::nodelete>>(m, "Renderer")
		.def_static("GetInstance", &Renderer::GetInstance)
		.def_readwrite("ShowText",&Renderer::mShowText)
		.def("RenderText", &Renderer::RenderText)
		.def("GetFadeMultiplier", &Renderer::GetFadeMultiplier)
		.def("Fade", &Renderer::Fade);

	py::class_<RandomNumberGenerator, std::unique_ptr<RandomNumberGenerator, py::nodelete>>(m, "RNG")
		.def_static("GetInstance", &RandomNumberGenerator::GetInstance)
		.def("GenerateFloat", &RandomNumberGenerator::GenerateFloat)
		.def("GenerateInteger", &RandomNumberGenerator::GenerateInteger);

	py::class_<LocalizationManager, std::unique_ptr<LocalizationManager, py::nodelete>>(m, "LocalizationManager")
		.def_static("GetInstance", &LocalizationManager::GetInstance)
		.def("GetText", &LocalizationManager::GetText, py::arg("aFile"), py::arg("aLang")= "");

	py::class_<PhysicsManager, std::unique_ptr<PhysicsManager, py::nodelete>>(m, "PhysicsManager")
		.def_static("GetInstance", &PhysicsManager::GetInstance)
		.def("GetCollisionManager", &PhysicsManager::GetCollisionManager, py::return_value_policy::reference)
		.def("OnCollisionEnter", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnCollisionEnter)

		.def("OnCollisionStay", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnCollisionStay)

		.def("OnCollisionExit", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnCollisionExit)

		.def("OnTriggerEnter", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnTriggerEnter)

		.def("OnTriggerStay", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnTriggerStay)

		.def("OnTriggerExit", (bool (PhysicsManager::*)(Collider*, Collider*))&PhysicsManager::OnTriggerExit);


		py::enum_<ContactType>(m, "ContactType", py::arithmetic())
			.value("NonCollision", ContactType::NonCollision)
			.value("CollisionEnter", ContactType::OnCollisionEnter)
			.value("CollisionExit", ContactType::OnCollisionExit)
			.value("CollisionStay", ContactType::OnCollisionStay)
			.value("TriggerEnter", ContactType::OnTriggerEnter)
			.value("TriggerExit", ContactType::OnTriggerExit)
			.value("TriggerStay", ContactType::OnTriggerStay)
			.export_values();



	py::class_<CameraManager,std::unique_ptr<CameraManager, py::nodelete>>(m, "CameraManager")
		.def_static("GetInstance", &CameraManager::GetInstance)
		.def("SetCurrentCamera", &CameraManager::SetCurrentCamera)
		.def("GetCameraID", &CameraManager::GetCameraID)
		.def("GetPos", &CameraManager::GetVectorCameraPosition)
		.def("GetFront", &CameraManager::GetVectorCameraFront);

	py::class_<RayCast, std::unique_ptr<RayCast, py::nodelete>>(m, "RayCast")
		.def_static("GetInstance", &RayCast::GetInstance)
		.def("MoveCrossHair", (void(RayCast::*)(float, float))&RayCast::MoveCrosshair)
		.def("CastRay", (void(RayCast::*)(float, float, bool))&RayCast::CastRay);


	py::class_<InputManager, std::unique_ptr<InputManager, py::nodelete>>(m, "InputManager")
		.def_static("GetInstance", &InputManager::GetInstance)
		.def("IsPressed", (bool (InputManager::*)(std::string, int)) &InputManager::IsPressed, py::arg("button"), py::arg("num") = -1)
		.def("IsTriggered", (bool (InputManager::*)(std::string, int)) &InputManager::IsTriggered, py::arg("button"), py::arg("num") = -1)
		.def("IsReleased", (bool (InputManager::*)(unsigned int)) &InputManager::IsReleased)
		.def("IsReleased", (bool (InputManager::*)(std::string, int)) &InputManager::IsReleased, py::arg("button"), py::arg("num") = -1);

	py::class_<EntityManager, std::unique_ptr<EntityManager, py::nodelete>>(m, "EntityManager")
		.def_static("GetInstance", &EntityManager::GetInstance)
		.def("GetEntity",&EntityManager::GetEntity, py::return_value_policy::reference)
		.def("ToggleEntityEnable",&EntityManager::EntityEnable);

	//Mouse
	py::class_<Mouse, std::unique_ptr<Mouse, py::nodelete>>(m, "Mouse")
		.def_static("GetInstance", &Mouse::GetInstance)
		.def("MouseDrag", (std::pair<float, float>(Mouse::*)(std::string)) &Mouse::MouseDrag)
		.def("MouseDrag", (std::pair<float, float>(Mouse::*)(int)) &Mouse::MouseDrag)
		.def("MouseDragY", (float (Mouse::*)(std::string)) &Mouse::MouseDragY)
		.def("MouseDragY", (float (Mouse::*)(int)) &Mouse::MouseDragY)
		.def("MouseDragX", (float (Mouse::*)(std::string)) &Mouse::MouseDragX)
		.def("MouseDragX", (float (Mouse::*)(int)) &Mouse::MouseDragX)
		.def("MouseDeltaX", &Mouse::GetMouseDeltaX)
		.def("MouseDeltaY", &Mouse::GetMouseDeltaY)
		.def("IsPressed", (bool (Mouse::*)(int))&Mouse::IsPressed)
		.def("IsTriggered", &Mouse::IsTriggered)
		.def("IsReleased", &Mouse::IsReleased)
		.def("ScrollUp", &Mouse::ScrollUp)
		.def("ScrollDown", &Mouse::ScrollDown)
		.def("GetScreenPos", (std::pair<float, float>(Mouse::*)())&Mouse::GetMousePos)
		.def("GetSensitivity", &Mouse::GetSensitivity);//*/

	py::class_<EnemyStateManager, std::unique_ptr<EnemyStateManager, py::nodelete>>(m, "EnemyStateManager")
		.def_static("GetInstance", &EnemyStateManager::GetInstance)
		.def_readwrite("mGameOver", &EnemyStateManager::mGameOver);

	py::class_<LevelEditor, std::unique_ptr<LevelEditor, py::nodelete>>(m, "LevelEditor")
		.def_static("GetInstance", &LevelEditor::GetInstance)
		.def("ChangeLevel", &LevelEditor::ChangeLevel);

	py::class_<ObjectFactory, std::unique_ptr<ObjectFactory, py::nodelete>>(m, "ObjectFactory")
		.def_static("GetInstance", &ObjectFactory::GetInstance)
		.def_readwrite("mChangeLevel", &ObjectFactory::mCurrentLevel);

	py::class_<SceneManager, std::unique_ptr<SceneManager, py::nodelete>>(m, "SceneManager")
		.def_static("GetInstance", &SceneManager::GetInstance)
		.def_readwrite("ScreenWidth", &SceneManager::mScreenWidth)
		.def_readwrite("ScreenHeight", &SceneManager::mScreenHeight);



	//Events
	
	py::class_<Event>(m, "Event")
		.def("GetType", &Event::GetType)
		.def("SetTime", &Event::SetTime)
		.def("GetTime", &Event::GetTime);

	py::class_<ResetEntityEvent, Event>(m, "ResetEntityEvent")
		.def("GetType", &ResetEntityEvent::GetType)
		.def("SetTime", &ResetEntityEvent::SetTime)
		.def("GetTime", &ResetEntityEvent::GetTime)
		.def_readwrite("mObjectId", &ResetEntityEvent::mObjectId)
		.def_readwrite("mResetX", &ResetEntityEvent::mResetX)
		.def_readwrite("mResetY", &ResetEntityEvent::mResetY)
		.def_readwrite("mResetZ", &ResetEntityEvent::mResetZ);

	py::class_<ContactEvent,Event>(m, "ContactEvent")
		.def("GetType", &ContactEvent::GetType)
		.def("SetTime", &ContactEvent::SetTime)
		.def("GetTime", &ContactEvent::GetTime)
		.def_readwrite("mContactType", &ContactEvent::mContactType)
		.def_readwrite("mContactPoint", &ContactEvent::mContactPoint)
		.def_readwrite("mContactNormal", &ContactEvent::mContactNormal)
		.def_readonly("mId1",&ContactEvent::mId1)
		.def_readonly("mId2", &ContactEvent::mId2);

	py::class_<AttackEvent, Event>(m, "AttackEvent")
		.def("GetType", &AttackEvent::GetType)
		.def("SetTime", &AttackEvent::SetTime)
		.def("GetTime", &AttackEvent::GetTime)
		.def_readwrite("mForce", &AttackEvent::mForceAmount)
		.def_readwrite("mAttack",&AttackEvent::mAttackId);

	py::class_<RecipeMatchEvent, Event>(m, "RecipeMatchEvent")
		.def_readwrite("mProcessorID", &RecipeMatchEvent::mProcessorID)
		.def_readwrite("mOutput", &RecipeMatchEvent::mOutput);

	py::class_<PlayerPickupEvent, Event>(m, "PlayerPickupEvent")
		.def_readwrite("mPlayerID", &PlayerPickupEvent::mPlayerID)
		.def_readwrite("mItemID", &PlayerPickupEvent::mItemID);

	py::class_<PlayerDropEvent, Event>(m, "PlayerDropEvent")
		.def_readwrite("mPlayerID", &PlayerDropEvent::mPlayerID);

	py::class_<ContainerInputEvent, Event>(m, "ContainerInputEvent")
		.def_readwrite("mPlayerID", &ContainerInputEvent::mPlayerID)
		.def_readwrite("mContainerID", &ContainerInputEvent::mContainerID)
		.def_readwrite("mNumItems", &ContainerInputEvent::mNumItems);

	py::class_<ContainerOutputEvent, Event>(m, "ContainerOutputEvent")
		.def_readwrite("mPlayerID", &ContainerOutputEvent::mPlayerID)
		.def_readwrite("mContainerID", &ContainerOutputEvent::mContainerID)
		.def_readwrite("mNumItems", &ContainerOutputEvent::mNumItems)
		.def_readwrite("mItemTypes", &ContainerOutputEvent::mItemTypes);

	py::class_<ContainerMatchEvent, Event>(m, "ContainerMatchEvent")
		.def_readwrite("mContainerID", &ContainerMatchEvent::mContainerID);

	py::class_<RequestFilledEvent, Event>(m, "RequestFilledEvent")
		.def_readwrite("mContainerID", &RequestFilledEvent::mContainerID)
		.def_readwrite("mRequestedGame", &RequestFilledEvent::mRequestedGame);

	py::class_<RequestMadeEvent, Event>(m, "RequestMadeEvent")
		.def_readwrite("mGame", &RequestMadeEvent::mGame);

	py::class_<GeneratorOutputEvent, Event>(m, "GeneratorOutputEvent")
		.def_readwrite("mGeneratorID", &GeneratorOutputEvent::mGeneratorID)
		.def_readwrite("mPlayerID", &GeneratorOutputEvent::mPlayerID)
		.def_readwrite("mOutputs", &GeneratorOutputEvent::mOutputs);

	py::class_<GeneratorInputEvent, Event>(m, "GeneratorInputEvent")
		.def_readwrite("mGeneratorID", &GeneratorInputEvent::mGeneratorID)
		.def_readwrite("mPlayerID", &GeneratorInputEvent::mPlayerID)
		.def_readwrite("mInputType", &GeneratorInputEvent::mInputType);

	py::class_<InputEvent, Event>(m, "InputEvent")
		.def_readwrite("mGeneratorID", &InputEvent::mGeneratorID);

	py::class_<SetCellEvent, Event>(m, "SetCellEvent")
		.def_readwrite("mGridID", &SetCellEvent::mGridID)
		.def_readwrite("mResource", &SetCellEvent::mResource);

	py::class_<CellUpdateEvent, Event>(m, "CellUpdateEvent")
		.def_readwrite("mPlayerID", &CellUpdateEvent::mPlayerID)
		.def_readwrite("mCellID", &CellUpdateEvent::mCellID)
		.def_readwrite("mMatch", &CellUpdateEvent::mMatch);

	py::class_<CellEjectEvent, Event>(m, "CellEjectEvent");

	py::class_<CreateGameEvent, Event>(m, "CreateGameEvent")
		.def_readwrite("mOutput", &CreateGameEvent::mOutput);

	py::class_<InitLevelEvent, Event>(m, "InitLevelEvent")
		.def_readwrite("mTime", &InitLevelEvent::mTime)
		.def_readwrite("mMusic", &InitLevelEvent::mMusic)
		.def_readwrite("mGames", &InitLevelEvent::mGames)
		.def_readwrite("mGrading", &InitLevelEvent::mGrading)
		.def_readwrite("mLevelNumber", &InitLevelEvent::mLevelNumber)
		.def_readwrite("mLevelName", &InitLevelEvent::mLevelName)
		.def_readwrite("mNumPlayers", &InitLevelEvent::mNumPlayers);

	py::class_<HighlightEvent, Event>(m, "HighlightEvent");

	py::class_<UpdateLevelEvent, Event>(m, "UpdateLevelEvent")
		.def_readwrite("mCurrentTime", &UpdateLevelEvent::mCurrentTime);
	
	py::class_<ChangeAnimationStateEvent, Event>(m, "ChangeAnimationStateEvent")
		.def_readwrite("mState", &ChangeAnimationStateEvent::mState)
		.def_readwrite("mID", &ChangeAnimationStateEvent::mID);


	py::class_<ResetGridEvent, Event>(m, "ResetGridEvent");

	py::class_<PauseEvent, Event>(m, "PauseEvent")
		.def_readwrite("mPause", &PauseEvent::mPause)
		.def("GetType", &PauseEvent::GetType)
		.def("SetTime", &PauseEvent::SetTime)
		.def("GetTime", &PauseEvent::GetTime);

	py::class_<LevelCompleteEvent, Event>(m, "LevelCompleteEvent")
		.def_readwrite("mGrade", &LevelCompleteEvent::mGrade)
		.def_readwrite("mScore", &LevelCompleteEvent::mScore);

	py::class_<Entity>(m, "Entity")
		.def("GetID", &Entity::GetID)
		.def("GetName", &Entity::GetName)
		.def("ToggleIsEnabled", &Entity::SetEnabled);

};
