#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "DirectionalLightComponent.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Transform.h"
#include "../Resources/JsonData.h"
#include "../Entity.h"

DRLightComponent::DRLightComponent() : Component("DRLightComponent")
{
	mIntensity = 1.0f;
	mLcolor = glm::vec3(1.0);
}

DRLightComponent::~DRLightComponent()
{

}

void DRLightComponent::Reset()
{
	mIntensity = 1.0f;
	mLcolor = glm::vec3(0.0);

	Component::Reset();
}

DRLightComponent* DRLightComponent::Create()
{
	return new DRLightComponent();
}

void DRLightComponent::Update(float a_deltaTime)
{

}

void DRLightComponent::DisplayDebug()
{
	ImGui::ColorEdit3("Color", &mLcolor[0]);
	ImGui::DragFloat("Intensity", &mIntensity, 0.1f, 0.0f, 32.0f, "%.1f");
}


void DRLightComponent::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Intensity"];
	if (!dataRoot.isNull())
		mIntensity = dataRoot.asFloat();

	dataRoot = a_root["LightColor"];
	if (!dataRoot.isNull()) {
		mLcolor.x = dataRoot[0].asFloat();
		mLcolor.y = dataRoot[1].asFloat();
		mLcolor.z = dataRoot[2].asFloat();
	}
}

void DRLightComponent::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["DRLightComponent"];

	if (fabsf(defaultRoot["Intensity"].asFloat() - mIntensity) > 0.001f || a_ignoreDefault)
		a_root["Intensity"] = mIntensity;

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

glm::mat4 DRLightComponent::GetViewMatrix()
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = GetDirection();

	return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0));
}

glm::mat4 DRLightComponent::GetPerspectiveMatrix()
{
	return glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
}

glm::vec3 DRLightComponent::GetDirection()
{
	glm::mat4 rotation = glm::mat4(1.0f);

	Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform", true);
	if (!pTr)
		pTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
	if (pTr)
		rotation = pTr->GetRotationMatrix();

	return glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
}

void DRLightComponent::HandleEvent(Event * a_pEvent)
{
	
}

GEAR_API void DRLightComponent::Clone(int objID)
{
	DRLightComponent *rhs = static_cast<DRLightComponent*>(COMPONENT_MANAGER->GetComponent<DRLightComponent>(objID, "DRLightComponent"));

	mIntensity = rhs->mIntensity;
	mLcolor = rhs->mLcolor;
}
