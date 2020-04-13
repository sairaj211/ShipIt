#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "SpotLight.h"
#include "Managers/EntityManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"
#include "Transform.h"

SpotLight::SpotLight() : Component("SpotLight")
{
	mIntensity = 1.0f;
	mRange = 1.0f;
	mSpotAngle = 30.0f;
	mColor = glm::vec3(1.0f);
}

SpotLight::~SpotLight()
{

}

void SpotLight::Reset()
{
	mIntensity = 1.0f;
	mRange = 1.0f;
	mSpotAngle = 30.0f;
	mColor = glm::vec3(1.0f);
	
	Component::Reset();
}

SpotLight* SpotLight::Create()
{
	return new SpotLight();
}

void SpotLight::Update(float a_deltaTime)
{
}

void SpotLight::DisplayDebug()
{
	ImGui::ColorEdit3("Color", &mColor[0]);
	ImGui::DragFloat("Range", &mRange, 0.5f, 0.0f, 0.0f, "%.1f");
	ImGui::DragFloat("SpotAngle", &mSpotAngle, 1.0f, 1.0f, 45.0f, "%.1f");
	ImGui::DragFloat("Intensity", &mIntensity, 0.1f, 0.0f, 32.0f, "%.1f");	
}


void SpotLight::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Intensity"];
	if (!dataRoot.isNull()) {
		mIntensity = dataRoot.asFloat();
	}

	dataRoot = a_root["Range"];
	if (!dataRoot.isNull())
		mRange = dataRoot.asFloat();

	dataRoot = a_root["SpotAngle"];
	if (!dataRoot.isNull())
		mSpotAngle = dataRoot.asFloat();

	dataRoot = a_root["Color"];
	if (!dataRoot.isNull()) {
		mColor.x = dataRoot[0].asFloat();
		mColor.y = dataRoot[1].asFloat();
		mColor.z = dataRoot[2].asFloat();
	}
}

void SpotLight::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["DRLightComponent"];

	if (fabsf(defaultRoot["Intensity"].asFloat() - mIntensity) > 0.001f || a_ignoreDefault)
		a_root["Intensity"] = mIntensity;

	if (fabsf(defaultRoot["Range"].asFloat() - mRange) > 0.001f || a_ignoreDefault)
		a_root["Range"] = mRange;

	if (fabsf(defaultRoot["SpotAngle"].asFloat() - mSpotAngle) > 0.001f || a_ignoreDefault)
		a_root["SpotAngle"] = mSpotAngle;

	if (fabsf(defaultRoot["Color"][0].asFloat() - mColor.x) > 0.001f ||
		fabsf(defaultRoot["Color"][1].asFloat() - mColor.y) > 0.001f ||
		fabsf(defaultRoot["Color"][2].asFloat() - mColor.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value color(Json::arrayValue);
		color.append(mColor.x);
		color.append(mColor.y);
		color.append(mColor.z);
		a_root["Color"] = color;
	}
}

glm::mat4 SpotLight::GetViewMatrix()
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 direction = GetDirection();

	Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	if (!pTr)
		pTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
	if (pTr)
		position = pTr->GetPosition();

	return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0));
}

glm::mat4 SpotLight::GetPerspectiveMatrix()
{
	return glm::perspective(glm::pi<float>() / 2.0f, 1.0f, 0.1f, 1000.0f);
}

glm::vec3 SpotLight::GetDirection()
{
	glm::mat4 rotation = glm::mat4(1.0f);

	Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform", true);
	if (!pTr)
		pTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
	if (pTr)
		rotation = pTr->GetRotationMatrix();

	return glm::vec3(rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
}

void SpotLight::HandleEvent(Event * a_pEvent)
{

}

 void SpotLight::Clone(int objID)
{
	SpotLight *rhs = static_cast<SpotLight*>(COMPONENT_MANAGER->GetComponent<SpotLight>(objID, "SpotLight"));

	mIntensity = rhs->mIntensity;
	mRange = rhs->mRange;
	mSpotAngle = rhs->mSpotAngle;
	mColor = rhs->mColor;
}

 void SpotLight::SetColor(glm::vec3 a_Color)
{
	 mColor[0] = a_Color[0] / 255.0f;
	 mColor[1] = a_Color[1] /255.0f;
	 mColor[2] = a_Color[2] /255.0f;
}

