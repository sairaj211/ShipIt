#pragma once

#include"Component.h"
#include "Managers/ParticleManager.h"

class InstancedParticle;

enum ParticleEffect
{
	Par_Invalid = -1,
	Par_Trail,
	Par_PCArt,
	Par_Confetti
};

/*! \struct Particle
 * \brief Struct definition for Particle
 * Contains essential variables for every particle
 * position, velocity, acceleration and color of each particle is stored here
*/
struct Particle
{
	glm::vec3			mPosition;
	glm::vec3			mVelocity;
	glm::vec3			mDirection;
	glm::vec4			mColor;
	glm::vec3			mSpeed;
	glm::vec3			mScale;
	glm::vec3			mOffset;
	float				mActiveTime;
	float				mCurrentActiveTime;
	float				mLife;
	ParticleEffect		mEffect;
	bool				m_IsActive;
	std::string			mMaterialName;

	Particle() :
		mPosition(glm::vec3(0.0f)),
		mVelocity(glm::vec3(0.0f)),
		mDirection(glm::vec3(0.0f)),
		mColor(glm::vec4(0.0f)),
		mSpeed(glm::vec3(0.0f)),
		mScale(glm::vec3(0.0f)),
		mOffset(glm::vec3(0.0f)),
		mLife(0.0f),
		mEffect(Par_Invalid),
		m_IsActive(false),
		mMaterialName("")
	{
	}
};

class ParticlesComponent : public Component
{
public:
	GEAR_API ParticlesComponent();
	GEAR_API ~ParticlesComponent();
	GEAR_API ParticlesComponent* Create();
	GEAR_API virtual void Init();
	GEAR_API void Reset();
	GEAR_API void UpdateParticle(Particle& pParticle);
	GEAR_API void Spawn(Particle& particle);
	GEAR_API void Update(float deltaTime);
	GEAR_API void DisplayDebug();
	GEAR_API void Serialize(Json::Value root);
	GEAR_API void DeSerialize(Json::Value& root, bool a_ignoreDefault = false);
	GEAR_API void SetRunning(bool isRunning);
	GEAR_API bool GetRunning();
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	GEAR_API void Clone(int objID);

public:
	glm::vec3					mOffset;
	glm::vec3					mRange;
	glm::vec3					mVelocity;
	glm::vec3					mDirection;
	glm::vec4					mColor;
	glm::vec3					mRotation;
	glm::vec3					mScale;
	float						mLife;
	int							mNumOfParticles;
	int							mActiveParticles;
	int							mMinParticles;
	float						mTime;
	float						mCurrentTime;
	bool						m_IsTexture;
	int							m_effect;
	std::string					m_strMaterialName;
	uint32_t					m_uLastUnusedParticle;
	std::vector<Particle>		m_Particles;
	InstancedParticle*			m_pInstPart;

private:
	bool m_isRunning;
};