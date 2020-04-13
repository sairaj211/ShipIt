#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "InstancedParticle.h"
#include "Resources/Material.h"
#include "Resources/Shader.h"
#include "Managers/CameraManager.h"
#include "Mesh.h"
#include "Resources/Model.h"
#include "Managers/ShapeGenerator.h"
#include "Managers/ResourceManager.h"
#include "Resources/Texture.h"

InstancedParticle::InstancedParticle()
{
}

InstancedParticle::~InstancedParticle()
{
}

bool InstancedParticle::Initialize(const char* a_MaterialName)
{
	std::string str = a_MaterialName;
	if (strcmp(a_MaterialName, "") == 0)
	{
		return false;
	}
	m_Material = ResourceManager::GetInstance()->LoadMaterial(str);
	return true;
}

void InstancedParticle::Draw()
{
	Shader* pShader = m_Material->mpShader;
	/*if (pShader == nullptr)
	{
		return;
	}*/
	pShader->Use();

	if (m_IsTexture)
	{
		Texture* pTexture = m_Material->mpDiffuseTexture;
		if (pTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pTexture->mTextureID);
		}
	}

	pShader->SetUniform("textureFlag", m_IsTexture);
	pShader->SetUniform("u_View", CAMERA_MANAGER->GetCameraViewMatrix());
	pShader->SetUniform("u_Proj", CAMERA_MANAGER->GetCameraProjectionMatrix());
	
	//for (int i = 0; i < m_InstancedParData.size(); ++i)
	//{
	//	pShader->SetUniformMatrix4fv("u_Model", m_InstancedParData[i].ModelMatrix);
	//	pShader->SetUniform4fv("ParticleColor", &m_InstancedParData[i].Color);
	//	AUMRenderer->GetShape()->RenderQuad();
	//}
	Mesh* pCubeMesh = ShapeGenerator::GetInstance()->GetShapeMesh("InstancedCube");

	glBindBuffer(GL_ARRAY_BUFFER, pCubeMesh->mInsatnceVBO);
	glBufferData(GL_ARRAY_BUFFER, m_InstancedParData.size() * sizeof(instancedData), m_InstancedParData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(pCubeMesh->mVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)m_InstancedParData.size());
	glBindVertexArray(0);

	m_InstancedParData.clear();
}