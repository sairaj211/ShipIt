#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ObjectFactory.h"
#include "Component.h"
#include "ResourceManager.h"
#include "../Resources/JsonData.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "../Engine.h"
#include "Entity.h"
#include "Renderer.h"
#include "Editor/LevelEditor.h"

ObjectFactory* ObjectFactory::mInstance = nullptr;

ObjectFactory::ObjectFactory()
{

}

ObjectFactory::~ObjectFactory()
{

}

void ObjectFactory::LoadLevel(std::string a_fileName)
{
	// Get level json root
	Json::Value root = ResourceManager::GetInstance()->LoadJsonData(a_fileName, "./Resources/Level/")->mRoot[a_fileName];
	// Check if root is empty
	if (root.isNull()) {
		LOG_ERROR("Failed to load level " + a_fileName);
		return;
	}
	
	//LoadEntities(root, false, -1,a_fileName);

	//LoadInnerReferences(false); // May cause issues with child entities

	std::vector<int> childIDs = LoadEntities(root, false, -1, a_fileName);
	for (int ids : childIDs) {
		std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(ids, true);

		for (Component* compo : components)
		{

			compo->Init();
		}
	}

	if(RENDERER->GetFadeMultiplier() <= 0.0f)
		RENDERER->Fade(true, 1.0f);

	mCurrentLevel = a_fileName;
}

std::vector<int> ObjectFactory::LoadEntities(Json::Value a_root, bool a_isRuntime, int a_parentID, std::string a_scene) {
	std::vector<int> newIDs;
	// Load and reserialize  entities
	for (size_t i = 0; i < a_root.size(); ++i) {
		// Get current entity root
		Json::Value entityRoot = a_root[int(i)];

		// Check if data file exists
		Json::Value dataFile = entityRoot["DataFile"];
		if (!dataFile.isNull()) {
			Entity* pNewEntity;
			if (a_isRuntime)
				pNewEntity = EntityManager::GetInstance()->CreateRuntimeEntity(dataFile.asString(), a_parentID,a_scene);
			else
				pNewEntity = EntityManager::GetInstance()->CreateEntity(dataFile.asString(), a_parentID,a_scene);
			newIDs.push_back(pNewEntity->GetID());
			// Reserialize new object
			pNewEntity->Serialize(entityRoot, a_isRuntime);
			SCENE_MANAGER->AddEntity(pNewEntity->GetID(), a_scene);
			// Reserialize new object's components
			std::vector<Component*> entityComponentList;
			if (a_isRuntime)
				entityComponentList = ComponentManager::GetInstance()->GetRuntimeComponents(pNewEntity->GetID());
			else
				entityComponentList = ComponentManager::GetInstance()->GetComponents(pNewEntity->GetID(), true);
			for (Component* pComponent : entityComponentList)
				pComponent->Serialize(entityRoot[pComponent->GetType()]);
		}
	}

	return newIDs;
}

void ObjectFactory::SaveLevel(std::string a_fileName)
{
// 	// TODO: Implement
	Json::StreamWriterBuilder streamBuilder;
	Json::StreamWriter* writer = streamBuilder.newStreamWriter();
	Json::Value root;

	Json::Value objects(Json::arrayValue);
	int i = 0;
	for (std::pair<int, Entity*> element : ENTITY_MANAGER->mEntityMap) {
		if (element.second) {
			if (element.second->GetParentID() == -1
				&& std::find(LEVEL_EDITOR->mRunTimeTimeList.begin(), LEVEL_EDITOR->mRunTimeTimeList.end(), element.second->GetID()) == LEVEL_EDITOR->mRunTimeTimeList.end()) {
				Json::Value object;
				object["DataFile"] = element.second->GetDataFile();
				element.second->DeSerialize(object);
				objects.append(object);
				i++;
			}
		}
	}
	root[a_fileName] = objects;
	//Deserializer
	std::ofstream output("Resources/Level/" + a_fileName + ".json");
	writer->write(root, &output);
	output.close();

	RESOURCE_MANAGER->ReloadJsonData(a_fileName);
}

void ObjectFactory::LoadInnerReferences(bool a_isRuntime)
{

	std::vector<Component*> components;
	if (a_isRuntime)
		components = COMPONENT_MANAGER->GetAllRuntimeComponents();
	else
		components = COMPONENT_MANAGER->GetAllComponents(true);

	for (Component* com : components)
	{
		com->Init();// initialize position and prevPosition, invMass
	}
}

ObjectFactory* ObjectFactory::GetInstance()
{
	if (!mInstance)
		mInstance = new ObjectFactory();

	return mInstance;
}

void ObjectFactory::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}