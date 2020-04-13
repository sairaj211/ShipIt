#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ParticleManager.h"
#include "../Managers/ComponentManager.h"
#include "../Components/ParticlesComponent.h"
#include "InstancedParticle.h"

ParticleManager* ParticleManager::mInstance = nullptr;

void ParticleManager::Draw()
{
	std::vector<Component*> ParticleComponent = ComponentManager::GetInstance()->GetComponents("ParticlesComponent");

	for (Component* pComponent : ParticleComponent)
	{
		ParticlesComponent* pParticle = static_cast<ParticlesComponent*>(pComponent);
		if (pParticle->m_pInstPart->m_InstancedParData.size() > 0)
		{
			pParticle->m_pInstPart->Draw();
		}
	}
}

void ParticleManager::CleanUp()
{
	delete mInstance;
}

ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{

}

ParticleManager* ParticleManager::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new ParticleManager();
	}
	return mInstance;
}