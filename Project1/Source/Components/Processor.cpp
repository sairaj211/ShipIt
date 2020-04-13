#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Processor.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Resources/Text.h"
#include "Entity.h"

Processor::Processor() : Component("Processor")
{
	Text* pRecipeText = RESOURCE_MANAGER->LoadText("ResourceRecipes");

	std::string recipeString;
	while (std::getline(pRecipeText->mFileStream, recipeString)) {
		std::vector<int> recipe;
		std::istringstream stringStream(recipeString);
		std::string resource;
		while (std::getline(stringStream, resource, ','))
			recipe.push_back(std::stoi(resource));
		mRecipeMap.push_back(recipe);
	}
	pRecipeText->mFileStream.clear();
	pRecipeText->mFileStream.seekg(0, std::ios::beg);

	mMaxInput = 0;
	mCurrentState = IDLE;
	mMaxProcessingTime = 0.0f;
	mCurrentProcessingTime = 0.0f;
}

Processor::~Processor()
{

}

void Processor::Reset()
{
	mMaxInput = 0;
	mRecipes.clear();
	mCurrentState = IDLE;
	mMaxProcessingTime = 0.0f;
	mCurrentProcessingTime = 0.0f;

	Component::Reset();
}

Processor* Processor::Create()
{
	return new Processor();
}

void Processor::Update(float a_deltaTime)
{
	if (mCurrentState == WORKING) {
		mCurrentProcessingTime += a_deltaTime * 0.001f;

		if (mCurrentProcessingTime > mMaxProcessingTime) {
			mCurrentState = DONE;
			mCurrentProcessingTime = 0.0f;
		}
	}
}

void Processor::DisplayDebug()
{
	std::string stateString = "Current State: ";
	if (mCurrentState == WORKING)
		stateString += "WORKING";
	else if (mCurrentState == IDLE)
		stateString += "IDLE";
	else if (mCurrentState == PAUSED)
		stateString += "PAUSED";
	else if (mCurrentState == DONE)
		stateString += "DONE";
	else
		stateString += "NONE";

	ImGui::Text(stateString.c_str());

	ImGui::InputInt("MaxInput", &mMaxInput, 0, 0);

	// TODO: Implement ImGui interface for processor recipes
	
	ImGui::InputFloat("MaxProcessingTime", &mMaxProcessingTime);

	ImGui::Text("CurrentProcessingTime: %.2f", mCurrentProcessingTime);
}

void Processor::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["MaxInput"];
	if (!dataRoot.isNull())
		mMaxInput = dataRoot.asInt();

	dataRoot = a_root["Recipes"];
	if (!dataRoot.isNull()) {
		mRecipes.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			int resource = dataRoot[i].asInt();
			std::vector<int> recipe = mRecipeMap[resource];
			std::unordered_map<int, int> recipeResources;
			for (int j = 0; j < recipe.size(); ++j) {
				if (recipeResources.count(recipe[j]) == 0)
					recipeResources[recipe[j]] = 1;
				else
					++recipeResources[recipe[j]];
			}
			mRecipes.push_back(std::pair<int, std::unordered_map<int, int>>(resource, recipeResources));
		}
	}

	dataRoot = a_root["MaxProcessingTime"];
	if (!dataRoot.isNull())
		mMaxProcessingTime = dataRoot.asFloat();
}

void Processor::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Processor"];

	if (mMaxInput != defaultRoot["MaxInput"].asInt() || a_ignoreDefault)
		a_root["MaxInput"] = mMaxInput;

	std::vector<int> defaultRecipes;
	for (unsigned int i = 0; i < defaultRoot["Recipes"].size(); ++i)
		defaultRecipes.push_back(defaultRoot["Recipes"][i].asInt());
	std::sort(defaultRecipes.begin(), defaultRecipes.end());
	bool diff = defaultRecipes.size() != mRecipes.size();
	if (!diff) {
		std::vector<int> recipes;
		for (int i = 0; i < mRecipes.size(); ++i)
			recipes.push_back(mRecipes[i].first);
		std::sort(recipes.begin(), recipes.end());
		diff = recipes != defaultRecipes;
	}
	if (diff || a_ignoreDefault) {
		Json::Value recipeArray(Json::arrayValue);
		for (int i = 0; i < mRecipes.size(); ++i)
			recipeArray.append(mRecipes[i].first);
		a_root["Recipes"] = recipeArray;
	}

	if (fabsf(defaultRoot["MaxProcessingTime"].asFloat() - mMaxProcessingTime) > 0.001f || a_ignoreDefault)
		a_root["MaxProcessingTime"] = mMaxProcessingTime;
}

void Processor::HandleEvent(Event* a_pEvent)
{

}

int Processor::GetMaxInput()
{
	return mMaxInput;
}

std::vector<std::pair<int, std::unordered_map<int, int>>> Processor::GetRecipes()
{
	return mRecipes;
}

bool Processor::IsIdle()
{
	return mCurrentState == IDLE;
}

bool Processor::IsWorking()
{
	return mCurrentState == WORKING;
}

bool Processor::IsDone()
{
	return mCurrentState == DONE;
}

bool Processor::IsPaused()
{
	return mCurrentState == PAUSED;
}

void Processor::Start()
{
	mCurrentState = WORKING;
	mCurrentProcessingTime = 0.0f;
}

void Processor::Stop()
{
	mCurrentState = IDLE;
	mCurrentProcessingTime = 0.0f;
}

void Processor::Pause()
{
	if (mCurrentState == WORKING)
		mCurrentState = PAUSED;
}

void Processor::Resume()
{
	if (mCurrentState == PAUSED)
		mCurrentState = WORKING;
}

float Processor::GetMaxProcessingTime()
{
	return mMaxProcessingTime;
}

float Processor::GetCurrentProcessingTime()
{
	return mCurrentProcessingTime;
}

void Processor::SetCurrentTime(float a_time)
{
	mCurrentProcessingTime = a_time;
}

GEAR_API void Processor::Clone(int objID)
{
	Processor *rhs = static_cast<Processor*>(COMPONENT_MANAGER->GetComponent<Processor>(objID, "Processor"));

	mMaxInput = rhs->mMaxInput;
	mMaxProcessingTime = rhs->mMaxProcessingTime;

}
