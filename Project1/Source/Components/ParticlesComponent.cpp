#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ParticlesComponent.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"
#include "../InstancedParticle.h"
#include "../Components/Transform.h"
#include "../Managers/FrameRateManager.h"
#include "../Engine.h"
#include <random>
#include <limits>

std::mt19937_64 RNGen;

//DIRECTION
std::uniform_real_distribution<> myrandomX(-1.0f, 1.0f);
std::uniform_real_distribution<> myrandomY(-5.0f, -1.0f);
std::uniform_real_distribution<> myrandomConfettiX(-50.0f, 50.0f);
std::uniform_real_distribution<> myrandomConfettiY(-10.0f, 10.0f);
std::uniform_real_distribution<> myrandomConfettiZ(-20.0f, 20.0f);

//SPEED
std::uniform_real_distribution<>myrandomVelocityForConfetti(0.01f, 0.05f);

//SCALE
std::uniform_real_distribution<> myrandomScaleConfetti(0.1f, 0.5f);
std::uniform_real_distribution<> myrandomConfettiYDown(5.0f, 10.0f);


//Colors
std::uniform_real_distribution<> myrandomColorX(0.0f, 0.9f);
std::uniform_real_distribution<> myrandomColorY(0.0f, 0.9f);
std::uniform_real_distribution<> myrandomColorZ(0.0f, 0.9f);

//Velocity_final
std::uniform_real_distribution<> myrandomVelocityConfetti(1.5f, 2.0f);
std::uniform_real_distribution<> myrandomVelocityConfettiYDown(0.09f, 0.3f);

ParticlesComponent::ParticlesComponent() : Component("ParticlesComponent")
{
	mOffset = glm::vec3(0);
	mRange = glm::vec3(0);
	mVelocity = glm::vec3(0);
	mDirection = glm::vec3(0);
	mColor = glm::vec4(1);
	mScale = glm::vec3(0);
	mLife = 0.0f;
	mNumOfParticles = 0;
	mActiveParticles = 0;
	m_strMaterialName = "";
	mMinParticles = 1;
	m_IsTexture = false;
	m_uLastUnusedParticle = 0;
	m_effect = (int)Par_Invalid;
	m_Particles.resize(2000);
	m_pInstPart = new InstancedParticle();
	m_isRunning = false;
	mTime = std::numeric_limits<float>::min();
	mCurrentTime = std::numeric_limits<float>::min();
}

ParticlesComponent::~ParticlesComponent()
{
	delete m_pInstPart;
}

GEAR_API void ParticlesComponent::HandleEvent(Event * a_pEvent)
{
	
}

void ParticlesComponent::Reset()
{
	mOffset = glm::vec3(0);
	mRange = glm::vec3(0);
	mVelocity = glm::vec3(0);
	mDirection = glm::vec3(0);
	mColor = glm::vec4(1);
	mScale = glm::vec3(0);
	mLife = 0.0f;
	mNumOfParticles = 0;
	mActiveParticles = 0;
	mMinParticles = 1;
	m_strMaterialName = "";
	m_IsTexture = true;
	m_uLastUnusedParticle = 0;
	m_effect = (int)Par_Invalid;
	mTime = std::numeric_limits<float>::min();
	mCurrentTime = std::numeric_limits<float>::min();

	Component::Reset();
}

ParticlesComponent* ParticlesComponent::Create()
{
	return new ParticlesComponent();
}

void ParticlesComponent::Init()
{
	m_pInstPart->Initialize(m_strMaterialName.c_str());
}

void ParticlesComponent::Spawn(Particle& particle)
{
	Transform* pTransform = ComponentManager::GetInstance()->GetComponent<Transform>(GetOwnerID(), "Transform");

	particle.m_IsActive = true;
	particle.mLife = mLife;
	particle.mColor = mColor;

	++mActiveParticles;
	m_pInstPart->m_IsTexture = m_IsTexture;
	m_pInstPart->Initialize(m_strMaterialName.c_str());

	float rangeMaxX = mRange.x;
	float rangeMaxY = mRange.y;
	float rangeMaxZ = mRange.z;
	std::uniform_real_distribution<> RangeX = std::uniform_real_distribution<>(0.0f, rangeMaxX);
	std::uniform_real_distribution<> RangeY = std::uniform_real_distribution<>(0.0f, rangeMaxY);
	std::uniform_real_distribution<> RangeZ = std::uniform_real_distribution<>(0.0f, rangeMaxZ);

	glm::vec3 position = pTransform->GetPosition();
	particle.mPosition.x = position.x + mOffset.x + (float)RangeX(RNGen);
	particle.mPosition.y = position.y + mOffset.y + (float)RangeY(RNGen);
	particle.mPosition.z = position.z + mOffset.z + (float)RangeZ(RNGen);

	if ((ParticleEffect)m_effect == Par_PCArt)
	{
		int randColor = 1 + (rand() % 3);
		if (randColor == 1)
		{
			particle.mColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
		}
		else if (randColor == 2)
		{
			particle.mColor = glm::vec4(1.0, 1.0, 0.0, 1.0);
		}
		else
		{
			particle.mColor = glm::vec4(0.0, 0.0, 1.0, 1.0);
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	//Confetti
	///////////////////////////////////////////////////////////////////////////////

	if ((ParticleEffect)m_effect == Par_Confetti)
	{
		particle.mDirection.x = position.x + (float)myrandomConfettiX(RNGen) + mDirection.x;
		particle.mDirection.y = (position.y + (float)myrandomConfettiY(RNGen)) + mDirection.y;
		particle.mDirection.z = (position.z + (float)myrandomConfettiZ(RNGen)) + mDirection.z;
		particle.mDirection = glm::normalize(particle.mDirection - particle.mPosition);
		particle.mScale =/* glm::vec3((float)myrandomScaleConfetti(RNGen)) + */mScale;
		particle.mVelocity = glm::vec3((float)myrandomVelocityConfetti(RNGen)) * mVelocity * particle.mDirection * 2.0f;
		particle.mColor = glm::vec4(myrandomColorX(RNGen), myrandomColorY(RNGen), myrandomColorZ(RNGen), 1.0f);
	}
	else
	{
		///////////////////////////////////////////////////////////////////////////////
		//Fountain
		///////////////////////////////////////////////////////////////////////////////
		particle.mDirection.x = position.x + (float)myrandomX(RNGen) + mDirection.x;
		particle.mDirection.y = -(position.y + (float)myrandomY(RNGen)) + mDirection.y;
		//particle.mDirection.z = (pTransform->mPos.z + (float)myrandomConfettiX(RNGen)) + pParticleComponent->mDirection.z;
		//particle.mDirection = glm::normalize(particle.mDirection - particle.mPosition);
		particle.mScale = glm::vec3((float)myrandomScaleConfetti(RNGen)) + mScale;
		particle.mVelocity = glm::vec3((float)myrandomVelocityForConfetti(RNGen)) + mVelocity;

		particle.mVelocity.y = (float)myrandomVelocityForConfetti(RNGen) + mVelocity.y;
		particle.mVelocity.z = (float)myrandomVelocityForConfetti(RNGen) + mVelocity.z;
		particle.mScale.y = (float)myrandomVelocityForConfetti(RNGen) + mScale.y;
		particle.mScale.z = (float)myrandomVelocityForConfetti(RNGen) + mScale.z;
	}

	/*Color c = colorArray[myrandomIndex(RNGen)];
	p.color.x = c.r;
	p.color.y = c.g;
	p.color.z = c.b;*/
}

void ParticlesComponent::Update(float deltaTime)
{
	if (m_strMaterialName.empty())
	{
		return;
	}

	if (GetRunning())
	{
		float frameTime = (float)FRAMERATE_MANAGER->GetFrameTime() / 1000.0f;
		if (mCurrentTime != std::numeric_limits<float>::min())
		{
			mCurrentTime -= frameTime;
			if (mCurrentTime <= 0.0f)
			{
				SetRunning(false);
				mCurrentTime = mTime;
				return;
			}
		}

		if ((int)mMinParticles == 0)
		{
			mMinParticles = 1;
		}
		int requiredParticles = (mNumOfParticles - mActiveParticles) / mMinParticles;

		// SPAWN REQUIRED PARTICLES PER FRAME

		// first start finding from last unused
		for (uint32_t j = m_uLastUnusedParticle; j < m_Particles.size() && requiredParticles >= 0; ++j)
		{
			if (m_Particles[j].m_IsActive == false)
			{
				--requiredParticles;
				Particle& particle = m_Particles[j];
				Spawn(particle);
			}
		}

		// if reached end of vector-- it means no alive particles in (LAST UNUSED, END)
		if (requiredParticles > 0)
		{
			m_uLastUnusedParticle = 0; // hence reset
		}

		for (uint32_t j = 0; j < m_Particles.size() && requiredParticles >= 0; ++j)
		{
			if (m_Particles[j].m_IsActive == false)
			{
				--requiredParticles;
				Particle& particle = m_Particles[j];
				Spawn(particle);
			}
		}

		// UPDATE PARTICLES
		for (uint32_t i = 0; i < m_Particles.size(); ++i)
		{
			Particle& particle = m_Particles[i];

			if (particle.m_IsActive == false)
			{
				continue;
			}

			if (particle.mLife <= 0)
			{
				particle.m_IsActive = false;
				m_uLastUnusedParticle = i;
				--mActiveParticles;

				continue;
			}

			if ((ParticleEffect)m_effect == Par_Confetti)
			{
				if (particle.mLife <= mLife * 2.0f / 3.0f)
				{
					particle.mPosition.y -= particle.mVelocity.y * frameTime * (float)myrandomVelocityConfettiYDown(RNGen);
					particle.mColor.a -= (frameTime);
				}
				else
				{
					particle.mPosition += frameTime * particle.mVelocity;
				}
				particle.mLife -= frameTime;
			}
			else
			{
				UpdateParticle(particle);
			}

			//set model matrix
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(particle.mPosition));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(particle.mScale));

			// push into vector of modeling matrix, color
			m_pInstPart->m_InstancedParData.push_back({ modelMatrix, particle.mColor });
		}
	}
}

void ParticlesComponent::UpdateParticle(Particle& pParticle)
{
	float frameTime = (float)FRAMERATE_MANAGER->GetFrameTime() / 1000.0f;
	pParticle.mPosition += frameTime * pParticle.mVelocity;
	pParticle.mLife -= frameTime;
	pParticle.mColor.a -= (frameTime);


}

void ParticlesComponent::DisplayDebug()
{
	ImGui::DragFloat3("Position", &mOffset[0], 0.35f);
	ImGui::DragFloat3("Scale", &mScale[0], 0.35f);
	ImGui::DragFloat3("Range", &mRange[0], 0.35f);
	ImGui::DragFloat3("Velocity", &mVelocity[0], 0.35f);
	ImGui::DragFloat3("Direction", &mDirection[0], 0.35f);
	ImGui::DragFloat4("Color", &mColor[0], 0.35f);
	ImGui::InputFloat("Life", &mLife);
	ImGui::InputFloat("Active Time", &mTime);
	ImGui::InputInt("Number of Particles", &mNumOfParticles);
	ImGui::InputInt("Minimum Number of Particles", &mMinParticles);
	ImGui::InputInt("Effect", &m_effect);
}


void ParticlesComponent::Serialize(Json::Value root)
{
	Json::Value dataRoot = root["Position"];
	if (!dataRoot.isNull()) {
		mOffset.x = dataRoot[0].asFloat();
		mOffset.y = dataRoot[1].asFloat();
		mOffset.z = dataRoot[2].asFloat();
	}

	dataRoot = root["Range"];
	if (!dataRoot.isNull()) {
		mRange.x = dataRoot[0].asFloat();
		mRange.y = dataRoot[1].asFloat();
		mRange.z = dataRoot[2].asFloat();
	}

	dataRoot = root["Velocity"];
	if (!dataRoot.isNull()) {
		mVelocity.x = dataRoot[0].asFloat();
		mVelocity.y = dataRoot[1].asFloat();
		mVelocity.z = dataRoot[2].asFloat();
	}
	
	dataRoot = root["Direction"];
	if (!dataRoot.isNull()) {
		mDirection.x = dataRoot[0].asFloat();
		mDirection.y = dataRoot[1].asFloat();
		mDirection.z = dataRoot[2].asFloat();
	}

	dataRoot = root["Color"];
	if (!dataRoot.isNull()) {
		mColor.x = dataRoot[0].asFloat();
		mColor.y = dataRoot[1].asFloat();
		mColor.z = dataRoot[2].asFloat();
		mColor.w = dataRoot[3].asFloat();
	}

	dataRoot = root["Scale"];
	if (!dataRoot.isNull()) {
		mScale.x = dataRoot[0].asFloat();
		mScale.y = dataRoot[1].asFloat();
		mScale.z = dataRoot[2].asFloat();
	}

	dataRoot = root["Life"];
	if (!dataRoot.isNull()) {
		mLife = dataRoot.asFloat();
	}

	dataRoot = root["Active Time"];
	if (!dataRoot.isNull()) {
		mTime = dataRoot.asFloat();
	}

	mCurrentTime = mTime;

	dataRoot = root["Number of Particles"];
	if (!dataRoot.isNull()) {
		mNumOfParticles = dataRoot.asInt();
	}

	dataRoot = root["Minimum Number Of Particles"];
	if (!dataRoot.isNull()) {
		mMinParticles = dataRoot.asInt();
	}

	dataRoot = root["Material Name"];
	if (!dataRoot.isNull()) {
		m_strMaterialName = dataRoot.asString();
	}

	dataRoot = root["Texture"];
	if (!dataRoot.isNull())
	{
		m_IsTexture = dataRoot.asBool();
	}

	dataRoot = root["Effect"];
	if (!dataRoot.isNull()) 
	{
		m_effect = dataRoot.asInt();
	}
}

void ParticlesComponent::DeSerialize(Json::Value& root, bool a_ignoreDefault)
{
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["ParticlesComponent"];

	if (fabsf(defaultRoot["Position"][0].asFloat() - mOffset.x) > 0.001f ||
		fabsf(defaultRoot["Position"][1].asFloat() - mOffset.y) > 0.001f ||
		fabsf(defaultRoot["Position"][2].asFloat() - mOffset.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value particlePosition(Json::arrayValue);
		particlePosition.append(mOffset.x);
		particlePosition.append(mOffset.y);
		particlePosition.append(mOffset.z);
		root["Position"] = particlePosition;
	}

	if (fabsf(defaultRoot["Range"][0].asFloat() - mRange.x) > 0.001f ||
		fabsf(defaultRoot["Range"][1].asFloat() - mRange.y) > 0.001f ||
		fabsf(defaultRoot["Range"][2].asFloat() - mRange.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value particleRange(Json::arrayValue);
		particleRange.append(mRange.x);
		particleRange.append(mRange.y);
		particleRange.append(mRange.z);
		root["Range"] = particleRange;
	}

	if (fabsf(defaultRoot["Velocity"][0].asFloat() - mVelocity.x) > 0.001f ||
		fabsf(defaultRoot["Velocity"][1].asFloat() - mVelocity.y) > 0.001f ||
		fabsf(defaultRoot["Velocity"][2].asFloat() - mVelocity.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value particleVelocity(Json::arrayValue);
		particleVelocity.append(mVelocity.x);
		particleVelocity.append(mVelocity.y);
		particleVelocity.append(mVelocity.z);
		root["Velocity"] = particleVelocity;
	}

	if (fabsf(defaultRoot["Direction"][0].asFloat() - mDirection.x) > 0.001f ||
		fabsf(defaultRoot["Direction"][1].asFloat() - mDirection.y) > 0.001f ||
		fabsf(defaultRoot["Direction"][2].asFloat() - mDirection.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value mainDirection(Json::arrayValue);
		mainDirection.append(mDirection.x);
		mainDirection.append(mDirection.y);
		mainDirection.append(mDirection.z);
		root["Direction"] = mainDirection;
	}

	if (fabsf(defaultRoot["Color"][0].asFloat() - mColor.x) > 0.001f ||
		fabsf(defaultRoot["Color"][1].asFloat() - mColor.y) > 0.001f ||
		fabsf(defaultRoot["Color"][2].asFloat() - mColor.z) > 0.001f ||
		fabsf(defaultRoot["Color"][3].asFloat() - mColor.w) > 0.001f ||
		a_ignoreDefault) {
		Json::Value Color(Json::arrayValue);
		Color.append(mDirection.x);
		Color.append(mDirection.y);
		Color.append(mDirection.z);
		root["Color"] = Color;
	}

	if (fabsf(defaultRoot["Scale"][0].asFloat() - mScale.x) > 0.001f ||
		fabsf(defaultRoot["Scale"][1].asFloat() - mScale.y) > 0.001f ||
		fabsf(defaultRoot["Scale"][2].asFloat() - mScale.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value Scale(Json::arrayValue);
		Scale.append(mDirection.x);
		Scale.append(mDirection.y);
		Scale.append(mDirection.z);
		root["Scale"] = Scale;
	}

	if (fabsf(defaultRoot["Life"].asFloat() - mLife) > 0.001f || a_ignoreDefault)
	{
		root["Life"] = mLife;
	}

	if (fabsf(defaultRoot["Active Time"].asFloat() - mTime) > 0.001f || a_ignoreDefault)
	{
		root["Active Time"] = mTime;
	}

	if (defaultRoot["Number of Particles"].asInt() - mNumOfParticles > 1 || a_ignoreDefault)
	{
		root["Number of Particles"] = mNumOfParticles;
	}

	if (defaultRoot["Minimum Number Of Particles"].asInt() - mMinParticles > 1 || a_ignoreDefault)
	{
		root["Minimum Number Of Particles"] = mMinParticles;
	}

	if (m_strMaterialName != defaultRoot["Material Name"].asString() || a_ignoreDefault)
	{
		root["Material Name"] = m_strMaterialName;
	}

	if (m_IsTexture != defaultRoot["Texture"].asBool() || a_ignoreDefault)
	{
		root["Texture"] = m_IsTexture;
	}

	if (defaultRoot["Effect"].asInt() - m_effect > 1 || a_ignoreDefault)
	{
		root["Effect"] = m_effect;
	}
}

void ParticlesComponent::SetRunning(bool isRunning)
{
	m_isRunning = isRunning;
}

bool ParticlesComponent::GetRunning()
{
	return m_isRunning;
}

void ParticlesComponent::Clone(int objID)
{
	ParticlesComponent *rhs = static_cast<ParticlesComponent*>(COMPONENT_MANAGER->GetComponent<ParticlesComponent>(objID, "ParticlesComponent"));

	mOffset = rhs->mOffset;
	mRange = rhs->mRange;
	mVelocity = rhs->mVelocity;
	mDirection = rhs->mDirection;
	mColor = rhs->mColor;
	mScale = rhs->mScale;
	mLife = rhs->mLife;
	mNumOfParticles = rhs->mNumOfParticles;
	mActiveParticles = rhs->mActiveParticles;
	mMinParticles = rhs->mMinParticles;
	m_strMaterialName = rhs->m_strMaterialName;
	mTime = rhs->mTime;
	m_IsTexture = rhs->m_IsTexture;
	m_uLastUnusedParticle = rhs->m_uLastUnusedParticle;
	m_effect = rhs->m_effect;
}