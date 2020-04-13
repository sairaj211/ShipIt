#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ResourceManager.h"
#include "Resources/JsonData.h"
#include "Resources/Texture.h"
#include "Resources/Shader.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/AffineAnimation.h"
#include "Resources/SkeletalAnimation.h"
#include "Resources/Text.h"
#include "Managers/ComponentManager.h"
#include "Components/ModelComponent.h"

ResourceManager* ResourceManager::mInstance = nullptr;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	// Cleanup Json Resources
	for (auto& element : mJsonData)
		delete element.second;
	mJsonData.clear();

	// Cleanup Material Resources
	for (auto& element : mMaterials)
		delete element.second;
	mMaterials.clear();

	// Cleanup Texture Resources
	for (auto& element : mTextures)
		delete element.second;
	mTextures.clear();

	// Cleanup Shader Resources
	for (auto& element : mShaders)
		delete element.second;
	mShaders.clear();

	// Cleanup Json Resources
	for (auto& element : mModels)
		delete element.second;
	mModels.clear();
	
	// Cleanup AffineAnimation Resources
	for (auto& element : mAffineAnimations)
		delete element.second;
	mAffineAnimations.clear();
	
	// Cleanup SkeletalAnimation Resources
	for (auto& element : mSkeletalAnimations)
		delete element.second;
	mSkeletalAnimations.clear();

	// Cleanup Text Resources
	for (auto& element : mText)
		delete element.second;
	mText.clear();
}

ResourceManager* ResourceManager::GetInstance()
{
	if (!mInstance)
		mInstance = new ResourceManager();
	
	return mInstance;
}

void ResourceManager::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

void ResourceManager::ReloadJsonData(const std::string& a_fileName, std::string a_path)
{
	JsonData* pJsonData = mJsonData[a_fileName];

	if (pJsonData) {
		pJsonData->Reset();
		pJsonData->Load();
	}
}

JsonData* ResourceManager::LoadJsonData(const std::string& a_fileName, std::string a_path)
{
	JsonData* pJsonData = mJsonData[a_fileName];

	// If the resource doesn't exist, create it
	if (!pJsonData) {
		pJsonData = new JsonData(a_fileName, a_path);
		pJsonData->Load();

		mJsonData[a_fileName] = pJsonData;
	}

	// If the resource failed to load, load default
	if (pJsonData->LoadFailed())
		return LoadJsonData("Default", a_path);
	
	return pJsonData;
}

Texture* ResourceManager::LoadTexture(const std::string& a_fileName)
{
	Texture* pTexture = mTextures[a_fileName];

	// If the resource doesn't exist, create it
	if (!pTexture) {
		pTexture = new Texture(a_fileName);
		pTexture->Load();

		mTextures[a_fileName] = pTexture;
	}

	// If the resource failed to load, load default
	if (pTexture->LoadFailed())
		return LoadTexture("Default");
	
	return pTexture;
}

Texture * ResourceManager::GenerateTexture(const std::string& a_fileName, int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height)
{
	Texture* pTexture = mTextures[a_fileName];

	// If the resource doesn't exist, create it
	if (!pTexture) {
		pTexture = new Texture(a_fileName);
		pTexture->LoadRandom(a_octave, a_per, a_scale, a_freq, a_res, a_width, a_height);

		mTextures[a_fileName] = pTexture;
	}

	// If the resource failed to load, load default
	if (pTexture->LoadFailed())
		return LoadTexture("Default");
	
	return pTexture;
}

Shader* ResourceManager::LoadShader(const std::string& a_fileName)
{
	Shader* pShader = mShaders[a_fileName];

	// If the resource doesn't exist, create it
	if (!pShader) {
		pShader = new Shader(a_fileName);
		pShader->Load();

		mShaders[a_fileName] = pShader;
	}

	// If the resource failed to load, load default
	if (pShader->LoadFailed())
		return LoadShader("Default");
	
	return pShader;
}

Material* ResourceManager::LoadMaterial(const std::string& a_fileName)
{
	Material* pMaterial = mMaterials[a_fileName];

	// If the resource doesn't exist, create it
	if (!pMaterial) {
		pMaterial = new Material(a_fileName);
		pMaterial->Load();

		mMaterials[a_fileName] = pMaterial;
	}

	// If the resource failed to load, load default
	if (pMaterial->LoadFailed())
		return LoadMaterial("Default");
	
	return pMaterial;
}

void ResourceManager::FreeMaterial(const std::string& a_fileName)
{
	if (a_fileName != "Default") {
		delete mMaterials[a_fileName];
		mMaterials.erase(a_fileName);
	}
	
}

Model* ResourceManager::LoadModel(const std::string& a_fileName)
{
	Model* pModel = mModels[a_fileName];

	// If the resource doesn't exist, create it
	if (!pModel) {
		pModel = new Model(a_fileName);
		pModel->Load();

		mModels[a_fileName] = pModel;
	}

	// If the resource failed to load, load default
	if (pModel->LoadFailed())
		return LoadModel("Quad");

	return pModel;
}

AffineAnimation* ResourceManager::LoadAffineAnimation(const std::string& a_fileName)
{
	AffineAnimation* pAffineAnimation = mAffineAnimations[a_fileName];

	// If the resource doesn't exist, create it
	if (!pAffineAnimation) {
		pAffineAnimation = new AffineAnimation(a_fileName);
		pAffineAnimation->Load();

		mAffineAnimations[a_fileName] = pAffineAnimation;
	}

	// If the resource failed to load, load default
	if (pAffineAnimation->LoadFailed())
		return LoadAffineAnimation("Default");

	return pAffineAnimation;
}

SkeletalAnimation* ResourceManager::LoadSkeletalAnimation(const std::string& a_fileName)
{
	SkeletalAnimation* pSkeletalAnimation = mSkeletalAnimations[a_fileName];

	// If the resource doesn't exist, create it
	if (!pSkeletalAnimation) {
		pSkeletalAnimation = new SkeletalAnimation(a_fileName);
		pSkeletalAnimation->Load();

		mSkeletalAnimations[a_fileName] = pSkeletalAnimation;
	}

	// If the resource failed to load, load default
	if (pSkeletalAnimation->LoadFailed())
		return LoadSkeletalAnimation("Default");

	return pSkeletalAnimation;
}

Text* ResourceManager::LoadText(const std::string& a_fileName)
{
	Text* pText = mText[a_fileName];

	// If the resource doesn't exist, create it
	if (!pText) {
		pText = new Text(a_fileName);
		pText->Load();

		mText[a_fileName] = pText;
	}

	// If the resource failed to load, load default
	if (pText->LoadFailed())
		return LoadText("Default");

	return pText;
}