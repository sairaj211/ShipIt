#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ModelComponent.h"
#include "Managers/ResourceManager.h"
#include "Managers/EntityManager.h"
#include "Managers/Renderer.h"
#include "Resources/JsonData.h"
#include "Entity.h"
#include "Resources/Model.h"
#include "Resources/Material.h"
#include "Random/RandomNumberGenerator.h"
#include "Managers/ShapeGenerator.h"
#include "Editor/LevelEditor.h"
#include "../Engine.h"
#include "Utility.h"

ModelComponent::ModelComponent() : Component("ModelComponent")
{
	mpModel = nullptr;
	mpMaterial = RESOURCE_MANAGER->LoadMaterial("Default");
	mIsRenderable = false;
	mCastsShadow = false;
	mDebugDraw = 0;
	mIsUI = false;
	mIsBillboard = false;
	mEditMeshMaterial = 0;
	drawCollider = false;
}

ModelComponent::~ModelComponent()
{

}

void ModelComponent::Reset()
{
	mpModel = nullptr;
	mpMaterial = RESOURCE_MANAGER->LoadMaterial("Default");
	mIsRenderable = false;
	mCastsShadow = false;
	mDebugDraw = 0;
	mIsUI = false;
	mIsBillboard = false;
	mMeshMaterials.clear();
	mEditMeshMaterial = 0;
	// Reset values of base component
	Component::Reset();
}

ModelComponent* ModelComponent::Create()
{
	return new ModelComponent();
}

void ModelComponent::Update(float a_deltaTime)
{
	int transparencyLevel = GetTransparency();
	if (transparencyLevel == 0 || transparencyLevel == 1)
		RENDERER->PushToForwardRenderQueue(this);
	if (transparencyLevel == 1 || transparencyLevel == 2)
		if (mIsRenderable && !mIsUI)
			RENDERER->PushToRenderQueue(this);
	if (mCastsShadow)
		RENDERER->PushToShadowRenderQueue(this);
	if (mDebugDraw)
		RENDERER->PushToDebugRenderQueue(this);
	if (mIsRenderable && mIsUI)
		RENDERER->PushToUIRenderQueue(this);
}

void ModelComponent::DisplayDebug()
{
	// Display Model options
	if (ImGui::BeginCombo("Model##Model", mpModel->mName.c_str())) {
		std::vector<std::string> models = GetUniqueDirectoryFiles(".\\Resources\\Models\\");
		std::vector<std::string> shapeTypes = SHAPE_GENERATOR->GetShapeTypes();
		models.insert(models.begin(), shapeTypes.begin(), shapeTypes.end());

		for (std::string fileName : models) {
			bool isSelected = (mpModel->mName == fileName);
			if (ImGui::Selectable(fileName.c_str(), isSelected)) {
				mpModel = RESOURCE_MANAGER->LoadModel(fileName);
				mMeshMaterials = std::vector<Material*>(mpModel->mMeshes.size(), nullptr);
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	// Display Material options
	std::vector<std::string> materials = GetUniqueDirectoryFiles(".\\Resources\\Materials\\");
	if (ImGui::BeginCombo("Material##Material", mpMaterial->mName.c_str())) {
		for (std::string fileName : materials) {
			bool isSelected = (mpMaterial->mName == fileName);
			if (ImGui::Selectable(fileName.c_str(), isSelected))
				mpMaterial = RESOURCE_MANAGER->LoadMaterial(fileName);
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}
	
	// Compose a list of mesh material names
	std::vector<std::string> meshMaterialNames;
	for (int i = 0; i < mMeshMaterials.size(); ++i) {
		meshMaterialNames.push_back("Material" + std::to_string(i));
	}
	
	// Display Mesh Material names
	std::string currentEditMeshMaterial = meshMaterialNames[mEditMeshMaterial];
	if (ImGui::BeginCombo("Mesh Material##MMaterialName", currentEditMeshMaterial.c_str())) {
		for (int i = 0; i < meshMaterialNames.size(); ++i) {
			std::string materialName = meshMaterialNames[i];
			bool isSelected = (currentEditMeshMaterial == materialName);
			if (ImGui::Selectable(materialName.c_str(), isSelected))
				mEditMeshMaterial = i;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	// Display available materials for the selected mesh material
	materials.push_back("None");
	std::string currentEditMeshMaterialType = "None";
	if (mMeshMaterials[mEditMeshMaterial])
		currentEditMeshMaterialType = mMeshMaterials[mEditMeshMaterial]->mName;
	if (ImGui::BeginCombo(std::string(meshMaterialNames[mEditMeshMaterial] + " Material##MMaterial").c_str(), currentEditMeshMaterialType.c_str())) {
		for (std::string fileName : materials) {
			bool isSelected = (currentEditMeshMaterialType == fileName);
			if (ImGui::Selectable(fileName.c_str(), isSelected)) {
				if (fileName == "None")
					mMeshMaterials[mEditMeshMaterial] = nullptr;
				else
					mMeshMaterials[mEditMeshMaterial] = RESOURCE_MANAGER->LoadMaterial(fileName);
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Checkbox("Is Renderable", &mIsRenderable);
	ImGui::Checkbox("Casts Shadow", &mCastsShadow);
	ImGui::Checkbox("Is UI", &mIsUI);
	ImGui::Checkbox("Is Billboard", &mIsBillboard);

	bool drawWireframe = mDebugDraw & DRAW_WIREFRAME;
	bool drawAABB = mDebugDraw & DRAW_AABB;
	bool drawVelocity = mDebugDraw & DRAW_VELOCITY;
	//drawCollider = mDebugDraw & DRAW_COLLIDER;

	ImGui::Checkbox("Wireframe", &drawWireframe);
	ImGui::Checkbox("AABB", &drawAABB);
	ImGui::Checkbox("Velocity", &drawVelocity);
	ImGui::Checkbox("Collider", &drawCollider);

	// Update DebugDraw bits
	mDebugDraw ^= (-int(drawWireframe) ^ mDebugDraw) & DRAW_WIREFRAME;
	mDebugDraw ^= (-int(drawAABB) ^ mDebugDraw) & DRAW_AABB;
	mDebugDraw ^= (-int(drawVelocity) ^ mDebugDraw) & DRAW_VELOCITY;
	mDebugDraw ^= (-int(drawCollider) ^ mDebugDraw) & DRAW_COLLIDER;
}

void ModelComponent::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Model"];
	if (!dataRoot.isNull()) {
		mpModel = RESOURCE_MANAGER->LoadModel(dataRoot.asString());
		mMeshMaterials = std::vector<Material*>(mpModel->mMeshes.size(), nullptr);
	}

	dataRoot = a_root["Material"];
	if (!dataRoot.isNull())
		mpMaterial = RESOURCE_MANAGER->LoadMaterial(dataRoot.asString());

	dataRoot = a_root["MeshMaterials"];
	if (!dataRoot.isNull()) {
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			Material* meshMaterial = nullptr;
			if (dataRoot[i].asString() != "None")
				meshMaterial = RESOURCE_MANAGER->LoadMaterial(dataRoot[i].asString());

			if (i < mMeshMaterials.size())
				mMeshMaterials[i] = meshMaterial;
			else
				mMeshMaterials.push_back(meshMaterial);
		}
	}

	dataRoot = a_root["Renderable"];
	if (!dataRoot.isNull())
		mIsRenderable = dataRoot.asBool();
	
	dataRoot = a_root["CastsShadow"];
	if (!dataRoot.isNull())
		mCastsShadow = dataRoot.asBool();

	dataRoot = a_root["IsUI"];
	if (!dataRoot.isNull())
		mIsUI = dataRoot.asBool();

	dataRoot = a_root["IsBillboard"];
	if (!dataRoot.isNull())
		mIsBillboard = dataRoot.asBool();
}

void ModelComponent::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["ModelComponent"];

	if (mpModel->mName != defaultRoot["Model"].asString() || a_ignoreDefault)
		a_root["Model"] = mpModel->mName;

	if (mpMaterial->mName != defaultRoot["Material"].asString() || a_ignoreDefault)
		a_root["Material"] = mpMaterial->mName;

	bool isDifferent = defaultRoot["MeshMaterials"].size() != mMeshMaterials.size();
	if (!isDifferent) {
		for (unsigned int i = 0; i < defaultRoot["MeshMaterials"].size(); ++i) {
			std::string name = "None";
			if (mMeshMaterials[i])
				name = mMeshMaterials[i]->mName;
			else if (defaultRoot["MeshMaterials"][i].asString() != name) {
				isDifferent = true;
				break;
			}
		}
	}

	if (isDifferent || a_ignoreDefault) {
		Json::Value meshMaterials(Json::arrayValue);
		for (int i = 0; i < mMeshMaterials.size(); ++i) {
			if (mMeshMaterials[i])
				meshMaterials.append(mMeshMaterials[i]->mName);
			else
				meshMaterials.append("None");
		}
		a_root["MeshMaterials"] = meshMaterials;
	}
	

	if (mIsRenderable != defaultRoot["Renderable"].asBool() || a_ignoreDefault)
		a_root["Renderable"] = mIsRenderable;
	
	if (mCastsShadow != defaultRoot["CastsShadow"].asBool() || a_ignoreDefault)
		a_root["CastsShadow"] = mCastsShadow;

	if (mIsUI != defaultRoot["IsUI"].asBool() || a_ignoreDefault)
		a_root["IsUI"] = mIsUI;

	if (mIsBillboard != defaultRoot["IsBillboard"].asBool() || a_ignoreDefault)
		a_root["IsBillboard"] = mIsBillboard;
}

void ModelComponent::MakeRainbow()
{
	for (unsigned int i = 0; i < mMeshMaterials.size(); ++i) {
		if (!mMeshMaterials[i])
			mMeshMaterials[i] = RESOURCE_MANAGER->LoadMaterial("Default");

		int transparent = RNG->GenerateInteger(0, 1);
		float alpha = 1.0f;
		if (transparent)
			alpha = 0.5f;

		mMeshMaterials[i]->mDiffuseColor = glm::vec4(RNG->GenerateInteger(0, 255) / 255.0f, RNG->GenerateInteger(0, 255) / 255.0f, RNG->GenerateInteger(0, 255) / 255.0f, alpha);
	}
}

void ModelComponent::Highlight() {
	Outline outline;
	outline.pModelComponent = this;
	outline.respectOcclusion = true;
	float lum = 0.299f * mpMaterial->mDiffuseColor.r + 0.587f * mpMaterial->mDiffuseColor.g + 0.114f * mpMaterial->mDiffuseColor.b;
	if (lum > 0.5f) {
		outline.color = mpMaterial->mDiffuseColor - glm::vec4(0.75f, 0.75f, 0.75f, 0.0f);
	}
	else {
		outline.color = mpMaterial->mDiffuseColor + glm::vec4(0.75f, 0.75f, 0.75f, 0.0f);
	}
	RENDERER->PushToOutlineQueue(outline);
}

void ModelComponent::ToggleDrawCollider()
{
	mDebugDraw ^= DRAW_COLLIDER;
}

GEAR_API void ModelComponent::HandleEvent(Event * a_pEvent)
{
}

int ModelComponent::GetTransparency()
{
	// TODO: Optimize this so that it does happen every frame
	bool hasTransparency = false;
	bool hasOpacity = false;
	for (int i = 0; i < mMeshMaterials.size(); ++i) {
		if (mMeshMaterials[i]) {
			if (mMeshMaterials[i]->mAdditiveBlending) {
				hasTransparency = true;
				hasOpacity = true;
			}
			else if (mMeshMaterials[i]->mDiffuseColor.a < 1.0f/* && !mMeshMaterials[i]->mpDiffuseTexture*/)
				hasTransparency = true;
			else
				hasOpacity = true;
		}
		else {
			if (mpMaterial->mAdditiveBlending) {
				hasTransparency = true;
				hasOpacity = true;
			}
			else if (mpMaterial->mDiffuseColor.a < 1.0f/* && !mpMaterial->mpDiffuseTexture*/ || mpMaterial->mAdditiveBlending)
				hasTransparency = true;
			else
				hasOpacity = true;
		}
	}

	if (!hasOpacity && hasTransparency)
		return 0;
	if (hasOpacity && hasTransparency)
		return 1;
	if (hasOpacity && !hasTransparency)
		return 2;

	return -1;
}

GEAR_API void ModelComponent::Clone(int objID)
{
	ModelComponent *rhs = static_cast<ModelComponent*>(COMPONENT_MANAGER->GetComponent<ModelComponent>(objID, "ModelComponent"));

	mpMaterial = rhs->mpMaterial;
	mpModel = rhs->mpModel;

	mMeshMaterials = rhs->mMeshMaterials;
	mIsRenderable = rhs->mIsRenderable;
	mCastsShadow = rhs->mCastsShadow;

	mDebugDraw = rhs->mDebugDraw;
	mIsUI = rhs->mIsUI;
}