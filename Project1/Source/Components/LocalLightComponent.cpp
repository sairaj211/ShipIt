#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "LocalLightComponent.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "../Resources/JsonData.h"
#include "../Entity.h"

LocalLightComponent::LocalLightComponent() : Component("LocalLightComponent")
{
	mIntensity = 1.0f;
	mRadius = 1.0f;
	mLcolor = glm::vec3(1.0);
}

LocalLightComponent::~LocalLightComponent()
{

}

void LocalLightComponent::Reset()
{
	mIntensity = 1.0f;
	mRadius = 1.0f;
	mLcolor = glm::vec3(0.0);

	Component::Reset();
}

LocalLightComponent* LocalLightComponent::Create()
{
	return new LocalLightComponent();
}

void LocalLightComponent::Update(float a_deltaTime)
{
	
}

void LocalLightComponent::DisplayDebug()
{
	ImGui::InputFloat("Radius", &mRadius);
	ImGui::ColorEdit3("Color", &mLcolor[0]);
	ImGui::DragFloat("Intensity", &mIntensity, 0.1f, 0.0f, 32.0f, "%.1f");
}


void LocalLightComponent::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Intensity"];
	if (!dataRoot.isNull())
		mIntensity = dataRoot.asFloat();
		
	dataRoot = a_root["Radius"];
	if (!dataRoot.isNull())
		mRadius = dataRoot.asFloat();

	dataRoot = a_root["LightColor"];
	if (!dataRoot.isNull()) {
		mLcolor.x = dataRoot[0].asFloat();
		mLcolor.y = dataRoot[1].asFloat();
		mLcolor.z = dataRoot[2].asFloat();
	}
}

void LocalLightComponent::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["LocalLightComponent"];

	if (fabsf(defaultRoot["Intensity"].asFloat() - mIntensity) > 0.001f || a_ignoreDefault)
		a_root["Intensity"] = mIntensity;

	if (fabsf(defaultRoot["Radius"].asFloat() - mRadius) > 0.001f || a_ignoreDefault)
		a_root["Radius"] = mRadius;

	if (fabsf(defaultRoot["LightColor"][0].asFloat() - mLcolor.x) > 0.001f ||
		fabsf(defaultRoot["LightColor"][1].asFloat() - mLcolor.y) > 0.001f ||
		fabsf(defaultRoot["LightColor"][2].asFloat() - mLcolor.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value mlcolor(Json::arrayValue);
		mlcolor.append(mLcolor.x);
		mlcolor.append(mLcolor.y);
		mlcolor.append(mLcolor.z);
		a_root["LightColor"] = mlcolor;
	}
}

GEAR_API void LocalLightComponent::HandleEvent(Event * a_pEvent)
{
	
}

GEAR_API void LocalLightComponent::Clone(int objID)
{
	LocalLightComponent *rhs = static_cast<LocalLightComponent*>(COMPONENT_MANAGER->GetComponent<LocalLightComponent>(objID, "LocalLightComponent"));
	
	mIntensity = rhs->mIntensity;
	mRadius = rhs->mRadius;
	mLcolor = rhs->mLcolor;
}

void LocalLightComponent::SetColor(glm::vec3 a_Color)
{
	mLcolor[0] = a_Color[0] / 255.0f;
	mLcolor[1] = a_Color[1] / 255.0f;
	mLcolor[2] = a_Color[2] / 255.0f;
}
