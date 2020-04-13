#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Block.h"
#include "Engine.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"

Block::Block() : Component("Block")
{
	mCanInput = true;
}

Block::~Block()
{

}

void Block::Reset()
{
	mCanInput = true;

	Component::Reset();
}

Block* Block::Create()
{
	return new Block();
}

void Block::Update(float a_deltaTime)
{

	// Still nothing
}

void Block::DisplayDebug()
{
	ImGui::Checkbox("Can Input", &mCanInput);
}

void Block::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["CanInput"];
	if (!dataRoot.isNull())
		mCanInput = dataRoot.asBool();
}

void Block::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Block"];

	if (mCanInput != defaultRoot["CanInput"].asBool() || a_ignoreDefault)
		a_root["CanInput"] = mCanInput;
}

void Block::HandleEvent(Event* a_pEvent)
{
	// Also nothing
}

bool Block::CanInput()
{
	return mCanInput;
}
