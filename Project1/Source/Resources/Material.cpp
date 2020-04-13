#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Material.h"
#include "Managers/ResourceManager.h"
#include "JsonData.h"
#include "Timer.h"
#include "Shader.h"

Material::Material(std::string a_name) : Resource(a_name)
{
	mpDiffuseTexture = nullptr;
	mpNormalMap = nullptr;
	mpShader = nullptr;
	mAdditiveBlending = false;
	mUseLighting = true;
	mDiffuseColor = glm::vec4(0.0f);
	mSpecularColor = glm::vec3(0.0f);
	mShininess = 1.0f;
}

Material::~Material()
{

}

void Material::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	// Load Json Text into String
	std::ifstream file(".\\Resources\\Materials\\" + mName + ".mat");
	std::string fileString;
	std::getline(file, fileString, (char)EOF);

	// Create Json Reader
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();

	// Parse Json String
	std::string errorString;
	if (!reader->parse(fileString.c_str(), fileString.c_str() + fileString.size(), &mRoot, &errorString)) {
		LOG_ERROR("Failed to load Material " + mName + ": " + errorString);
		mLoadFailed = true;
		return;
	}

	// If the root is empty, the load has failed
	if (mRoot.isNull()) {
		LOG_ERROR("Failed to load Material " + mName + ": Material file is empty");
		mLoadFailed = true;
		return;
	}

	// Fill material values
	// Load the material's diffuse texture
	Json::Value dataRoot = mRoot["DiffuseTexture"];
	if (!dataRoot.isNull())
		mpDiffuseTexture = ResourceManager::GetInstance()->LoadTexture(dataRoot.asString());

	dataRoot = mRoot["NormalMap"];
	if (!dataRoot.isNull())
		mpNormalMap = ResourceManager::GetInstance()->LoadTexture(dataRoot.asString());

	// Load the material's diffuse color
	dataRoot = mRoot["Diffuse"];
	if (!dataRoot.isNull()) {
		mDiffuseColor.x = dataRoot[0].asFloat();
		mDiffuseColor.y = dataRoot[1].asFloat();
		mDiffuseColor.z = dataRoot[2].asFloat();
		mDiffuseColor.w = dataRoot[3].asFloat();
	}

	// Load the material's specular color
	dataRoot = mRoot["Specular"];
	if (!dataRoot.isNull()) {
		mSpecularColor.x = dataRoot[0].asFloat();
		mSpecularColor.y = dataRoot[1].asFloat();
		mSpecularColor.z = dataRoot[2].asFloat();
	}

	// Load the material shininess value
	dataRoot = mRoot["Shininess"];
	if (!dataRoot.isNull()) {
		mShininess = dataRoot.asFloat();
		if (mShininess < 1.0f)
			mShininess = 1.0f;
	}

	// Load the shader name
	dataRoot = mRoot["Shader"];
	if (!dataRoot.isNull())
		mpShader = RESOURCE_MANAGER->LoadShader(dataRoot.asString());

	// Load AdditiveBlending Flag
	dataRoot = mRoot["AdditiveBlending"];
	if (!dataRoot.isNull())
		mAdditiveBlending = dataRoot.asBool();

	// Load UseLighting Flag
	dataRoot = mRoot["UseLighting"];
	if (!dataRoot.isNull())
		mUseLighting = dataRoot.asBool();
}

std::vector<float> Material::GetTupleDiffuseColor()
{
	return std::vector<float>{mDiffuseColor.x, mDiffuseColor.y, mDiffuseColor.z};
}