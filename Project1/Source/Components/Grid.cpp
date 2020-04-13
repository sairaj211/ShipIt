#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Grid.h"
#include "Engine.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/EventManager.h"
#include "Managers/SceneManager.h"
#include "Resources/JsonData.h"
#include "Resources/Text.h"
#include "Resources/Material.h"
#include "Events/CreateGameEvent.h"
#include "Events/CellUpdateEvent.h"
#include "Events/RequestMadeEvent.h"
#include "Events/InitLevelEvent.h"
#include "Events/SetCellEvent.h"
#include "Events/CellEjectEvent.h"
#include "Event.h"
#include "Entity.h"
#include "Utility.h"

Grid::Grid() : Component("Grid")
{
	Text* pRecipeText = RESOURCE_MANAGER->LoadText("GameRecipes");

	std::string recipeString;
	//std::getline(pRecipeText->mFileStream, recipeString);

	std::list<std::vector<int>> tempList;
	char temp = 'E';
	while (std::getline(pRecipeText->mFileStream, recipeString)) {
		std::vector<int> recipe;
		std::istringstream stringStream(recipeString);
		std::string resource;
		std::istringstream abc(recipeString);
		char c1 = abc.get();
		char c2;
		abc.get(c2);
		while (std::getline(stringStream, resource, ',')) {
			if((resource.c_str())[0] != c1)
				recipe.push_back(std::stoi(resource));
		}
		if (c1 != temp)
		{
			recipeMap.insert(std::pair(temp, tempList));
			tempList.clear();
			temp = c1;
		}
		RecipeList.push_back(recipe);
		mRecipeMap.push_back(recipe);
		tempList.push_back(recipe);
	}
	recipeMap.insert(std::pair(temp, tempList));
	tempList.clear();

	pRecipeText->mFileStream.clear();
	pRecipeText->mFileStream.seekg(0, std::ios::beg);

//	SceneManager::GetInstance()->mRecipeMap = recipeMap;


	mCurrentRecipe = -1;
	mPlayerID = -1;
}

Grid::~Grid()
{

}

void Grid::Reset()
{
	mMatchList.clear();
	mCurrentRecipe = -1;
	mPlayerID = -1;

	Component::Reset();
}

Grid* Grid::Create()
{
	return new Grid();
}

void Grid::Update(float a_deltaTime)
{

	bool match = mMatchList.size() > 0;
	for (int i = 0; i < mMatchList.size(); ++i)
		match &= mMatchList[i];
	
	if (match) {
		// Notify the grid cells that a game is being created and disable children
		CreateGameEvent* pCreateGameEvent = static_cast<CreateGameEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("CreateGameEvent"));
		std::vector<int> children = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs();
		for (int i = 0; i < children.size(); ++i) {
			Entity* pChild = ENTITY_MANAGER->GetEntity(children[i], true);
			pChild->SetAllEnabled(true);
			EventManager::GetInstance()->SendEvent(pCreateGameEvent, children[i]);
			ENTITY_MANAGER->EntityEnable(children[i], false);
		}

		// Create game
		Entity* pGame = ENTITY_MANAGER->CreateRuntimeEntity(DecodeGame(mCurrentRecipe));
		Body* pNewBody = COMPONENT_MANAGER->GetRuntimeComponent<Body>(pGame->GetID(), "Body");
		if (pNewBody) {
			pNewBody->Init();
		}
		Transform* pNewTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pGame->GetID(), "Transform");
		Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
		glm::vec3 spawnLocation = glm::vec3(0.0f);
		if (pTr)
			spawnLocation = pTr->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
		/*ParticlesComponent* pParticles = COMPONENT_MANAGER->GetComponent<ParticlesComponent>(mOwnerID, "ParticlesComponent");
		if (pParticles) {
			ModelComponent* pNewModel = COMPONENT_MANAGER->GetRuntimeComponent<ModelComponent>(pGame->GetID(), "ModelComponent");
			glm::vec3 particleColor = glm::vec3(0.0f);
			if (pNewModel)
				particleColor = pNewModel->mpMaterial->mDiffuseColor;

			pParticles->SetRunning(true);
			pParticles->r_low = int(particleColor.x * 255.0 * 0.5);
			pParticles->r_high = int(particleColor.x * 255.0);
			pParticles->g_low = int(particleColor.y * 255.0 * 0.5);
			pParticles->g_high = int(particleColor.y * 255.0);
			pParticles->b_low = int(particleColor.z * 255.0 * 0.5);
			pParticles->b_high = int(particleColor.z * 255.0);
			pParticles->SetPosition(spawnLocation.x, spawnLocation.y, spawnLocation.z);
		}*/

		PlayerPickupEvent* pPlayerPickupEvent = static_cast<PlayerPickupEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("PlayerPickupEvent"));
		pPlayerPickupEvent->mPlayerID = mPlayerID;
		pPlayerPickupEvent->mItemID = pGame->GetID();
		EventManager::GetInstance()->SendEvent(pPlayerPickupEvent, mPlayerID);

		// Clear match list
		mMatchList.clear();
	}
}

void Grid::DisplayDebug()
{
	
}

void Grid::HandleEvent(Event* a_pEvent)
{
	if (a_pEvent->GetType() == "ResetGridEvent") {
		std::vector<int> children = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs();
		std::vector<int> recipe = GetRecipe(mCurrentRecipe);
		mMatchList.clear();
		for (int i = 0; i < recipe.size() && i < children.size(); ++i) {
			ENTITY_MANAGER->EntityEnable(children[i], true);

			mMatchList.push_back(false);
		}
	}
	if (a_pEvent->GetType() == "CellUpdateEvent") {
		CellUpdateEvent* pCellUpdateEvent = static_cast<CellUpdateEvent*>(a_pEvent);
		mPlayerID = pCellUpdateEvent->mPlayerID;
		std::vector<int> children = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs();
		for (int i = 0; i < children.size(); ++i)
			if (children[i] == pCellUpdateEvent->mCellID)
				mMatchList[i] = pCellUpdateEvent->mMatch;
	}
	if (a_pEvent->GetType() == "RequestMadeEvent") {
		RequestMadeEvent* pRequestMadeEvent = static_cast<RequestMadeEvent*>(a_pEvent);
		mCurrentRecipe = pRequestMadeEvent->mGame;
		std::string gameName = DecodeGame(mCurrentRecipe);

		// Change Material
		ModelComponent* pModel = COMPONENT_MANAGER->GetComponent<ModelComponent>(mOwnerID, "ModelComponent");
		if (pModel) {
			JsonData* pJsonData = RESOURCE_MANAGER->LoadJsonData(gameName, ".\\Resources\\Prefab\\");
			Json::Value material = pJsonData->mRoot["ModelComponent"]["Material"];
			if (!material.isNull())
				pModel->mpMaterial = RESOURCE_MANAGER->LoadMaterial(material.asString());
		}

		// Set Cell Info
		std::vector<int> children = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs();
		std::vector<int> recipe = GetRecipe(mCurrentRecipe);
		for (int i = 0; i < recipe.size() && i < children.size(); ++i) {
			// This is a strange exception to the enabling rule.  Might want to revisit later.
			Entity* pChild = ENTITY_MANAGER->GetEntity(children[i], true);
			if (!pChild->IsEnabled()) {
				pChild->SetEnabled(true);
				COMPONENT_MANAGER->SetComponentsEnabled(children[i], true);
			}
			
			SetCellEvent* pSetCellEvent = static_cast<SetCellEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("SetCellEvent"));
			pSetCellEvent->mGridID = mOwnerID;
			pSetCellEvent->mResource = recipe[i];
			EventManager::GetInstance()->SendEvent(pSetCellEvent, children[i]);

			mMatchList.push_back(false);
		}
	}
}

std::vector<int> Grid::GetRecipe(int a_game)
{
	//return RecipeList[a_game];
	return RecipeList[a_game];
}

int Grid::GetCurrentRecipe()
{
	return mCurrentRecipe;
}
