#include"pch.h"

#include "Core.h"
#include "Renderer.h"
#include "Resources/Shader.h"
#include "Resources/Model.h"
#include "Resources/Material.h"
#include "Resources/Texture.h"
#include "ResourceManager.h"

#include "Random/Noise.h"

#include "Physics/Shape.h"
#include "Physics/AABBTree.h"
#include "Editor/LevelEditor.h"

#include "Components/ModelComponent.h"
#include "Components/Transform.h"
#include "Components/Animator.h"
#include "Components/Body.h"
#include "Components/Collider.h"
#include "Components/LocalLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SpotLight.h"
#include "Components/UIComponent.h"

#include "Events/UIActionEvent.h"

#include "InputManager.h"
#include "ParticleManager.h"
#include "LocalizationManager.h"
#include "Log.h"
#include "ComponentManager.h"
#include "MemoryManager.h"
#include "EventManager.h"
#include "FrameRateManager.h"
#include "Mesh.h"
#include "FBO.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Engine.h"
#include "CameraManager.h"
#include "Timer.h"
#include "Utility.h"

#include <stb_image.h>

#include <freetype/ttnameid.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "ImGui/imgui_internal.h"

#define DEPTH_INDEX    0
#define COLOR_INDEX    0
#define POSITION_INDEX 0
#define NORMAL_INDEX   1
#define DIFFUSE_INDEX  2
#define SPECULAR_INDEX 3
#define LOCALIZATION LocalizationManager::GetInstance()

//int ENGINE_WIDTH = 1920, ENGINE_HEIGHT = 1080;

Renderer* Renderer::mInstance = nullptr;

FT_Library ft;
FT_Face face;


Renderer::Renderer()
{

}

Renderer::~Renderer() 
{
	glDeleteVertexArrays(1, &mDebugVAO);
	glDeleteBuffers(1, &mDebugVBO);
	glDeleteBuffers(1, &mDebugEBO);

	glDeleteVertexArrays(1, &mTextVAO);
	glDeleteBuffers(1, &mTextVBO);
	glDeleteBuffers(1, &mTextEBO);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

Renderer* Renderer::GetInstance()
{
	if (!mInstance)
		mInstance = new Renderer();

	return mInstance;
}

void Renderer::Cleanup() 
{
	delete mInstance;
}

void Renderer::Init() 
{
	LOG_INFO("Renderer Init");

	// Create FBO for shadow map
	mpDepthFBO = new FBO(1920, 1080, 1, true);
	mpGBuffer = new FBO(1920, 1080, 4, false);
	mpPBuffer = new FBO(1920, 1080, 1, false);
	mpSilhouetteBuffer = new FBO(1920, 1080, 1, false);

	mpBufferThread1 = new FBO(1920, 1080, 1, false);
	mpBufferThread2 = new FBO(1920, 1080, 1, false);

	// Enable Anti-Aliasing
	mAntiAliasing = true;

	// Shaders
	mpShadowShader = RESOURCE_MANAGER->LoadShader("Shadow");
	mpLocalShader = RESOURCE_MANAGER->LoadShader("LocalLighting");
	mpGlobalShader = RESOURCE_MANAGER->LoadShader("DRLighting");
	mpSpotShader = RESOURCE_MANAGER->LoadShader("SpotLight");
	mpGBufferShader = RESOURCE_MANAGER->LoadShader("GBuffer");
	mpAmbientShader = RESOURCE_MANAGER->LoadShader("Ambient");
	mpPBufferShader = RESOURCE_MANAGER->LoadShader("PBuffer");
	mpForwardShader = RESOURCE_MANAGER->LoadShader("Forward");
	mpLineShader = RESOURCE_MANAGER->LoadShader("Line");
	mpOutlineShader = RESOURCE_MANAGER->LoadShader("Outline");
	mpTextShader = RESOURCE_MANAGER->LoadShader("Text");
	mpDefaultShader = RESOURCE_MANAGER->LoadShader("Default");


	// Define the viewport dimensions
	float w, h;
	Engine::GetWindowSize(w, h);
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));

	// Clear buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// enable alpha support
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	// Enable Polygon Offset
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1, 0);

	// Set Debug Line Width
	glLineWidth(2);

	// Generate Debug Buffers
	glGenVertexArrays(1, &mDebugVAO);
	glGenBuffers(1, &mDebugVBO);
	glGenBuffers(1, &mDebugEBO);


	glBindVertexArray(mDebugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// Generate Text Buffers
	glGenVertexArrays(1, &mTextVAO);
	glGenBuffers(1, &mTextVBO);

	glBindVertexArray(mTextVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//INIT FREETYPE
	// FreeType
	
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		LOG_CRITICAL("ERROR::FREETYPE: Could not init FreeType Library");

	// Set FXAA Parameters
	mSpanMax = 8.0f;
	mReduceMin = 0.016f;
	mReduceMultiplier = 0.016f;

	// Load font as face
	ReloadFonts(".\\Resources\\Fonts\\arial.ttf");


	//Loading SCREEN TEST
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	mFadeMultiplier = 1.0f;
	mFadeTime = 0.0f;
	mFadeMaxTime = 0.0f;
	mFade = false;
	mFadeIn = false;
	// << std::endl;
	LOG_INFO(glGetString(GL_VERSION));
}

void Renderer::Draw()
{
	TIMER->Start("GBuffer");
	GBufferRender();
	TIMER->End();
 	AmbientRender();
	
 	GlobalRender();
	
 	LocalRender();
	TIMER->Start("Forward");
 	ForwardRender();
	TIMER->End();
 	PBufferRender();
	
	UIRender();
	
	
}

void Renderer::PushToRenderQueue(ModelComponent* a_pModelComponent) 
{
	mRenderable.push(a_pModelComponent);
}

void Renderer::PushToOutlineQueue(Outline a_outline)
{
	mOutlineQueue.push(a_outline);
}

void Renderer::PushToShadowRenderQueue(ModelComponent* a_pModelComponent) 
{
	mShadowRenderable.push_back(a_pModelComponent);
}

void Renderer::PushToForwardRenderQueue(ModelComponent* a_pModelComponent) 
{
	mForwardRenderable.push_back(a_pModelComponent);
}

void Renderer::PushToDebugRenderQueue(ModelComponent* a_pModelComponent) 
{
	mDebugRenderable.push_back(a_pModelComponent);
}

void Renderer::PushToUIRenderQueue(ModelComponent* a_pModelComponent) 
{
	mUIQueue.push(a_pModelComponent);
}

void Renderer::ShadowRender(glm::mat4 a_viewMatrix, glm::mat4 a_projectionMatrix)
{
	mpShadowShader->Use();
	mpDepthFBO->Bind();

	glViewport(0, 0, mpDepthFBO->mWidth, mpDepthFBO->mHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable Front Face Culling
	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);*/

	// Render each model from the perspective of the given light
	for (ModelComponent* pCurrentModelComponent : mShadowRenderable) {
		// Setup MVP Matrix
		mpShadowShader->SetUniform("modelMatrix", ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform")->ComputeModelMatrix());
		mpShadowShader->SetUniform("viewMatrix", a_viewMatrix);
		mpShadowShader->SetUniform("projectionMatrix", a_projectionMatrix);

		// Set Animation Matrices
		glm::mat4 affineAnimationMatrix = glm::mat4(1.0f);
		std::vector<glm::mat4> transformations;
		Animator* pAnimator = ComponentManager::GetInstance()->GetComponent<Animator>(pCurrentModelComponent->GetOwnerID(), "Animator");
		if (pAnimator) {
			pAnimator->GetAffineAnimationMatrix();
			if (pCurrentModelComponent->mpModel->mpSkeleton) {
				transformations = pAnimator->GetSkeletalTransformations(pCurrentModelComponent->mpModel);
			}
		}

		mpShadowShader->SetUniform("isAnimated", !transformations.empty());

		for (unsigned int i = 0; i < transformations.size(); ++i) {
			std::string uniformName = "boneTransformations[" + std::to_string(i) + "]";
			mpShadowShader->SetUniform(uniformName, transformations[i]);
		}

		mpShadowShader->SetUniform("affineAnimationMatrix", affineAnimationMatrix);

		// Draw 
		std::vector<Mesh*> meshes = pCurrentModelComponent->mpModel->mMeshes;
		for (int i = 0; i < meshes.size(); ++i) {
			// Get Material
			Material* pMaterial = pCurrentModelComponent->mpMaterial;
			if (pCurrentModelComponent->mMeshMaterials[i])
				pMaterial = pCurrentModelComponent->mMeshMaterials[i];

			// Set Material values
			Texture* pDiffuseTexture = pMaterial->mpDiffuseTexture;
			if (pDiffuseTexture) {
				pDiffuseTexture->Bind(mpShadowShader, "modelTexture", 0);
				mpShadowShader->SetUniform("UsesTexture", true);
			}
			else {
				mpShadowShader->SetUniform("UsesTexture", false);
			}

			mpShadowShader->SetUniform("diffuse", pMaterial->mDiffuseColor.x, pMaterial->mDiffuseColor.y, pMaterial->mDiffuseColor.z, pMaterial->mDiffuseColor.w);

			// Draw Mesh
			glBindVertexArray(meshes[i]->mVAO);
			glDrawElements(GL_TRIANGLES, (int)meshes[i]->mIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			if (pDiffuseTexture)
				pDiffuseTexture->Unbind();
		}
	}

	//glDisable(GL_CULL_FACE);

	mpShadowShader->Unuse();
	mpDepthFBO->Unbind();
}

void Renderer::GBufferRender() 
{
	mpGBufferShader->Use();
	mpGBuffer->Bind();

	glViewport(0, 0, mpGBuffer->mWidth, mpGBuffer->mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	// Pop first component to be rendered
	ModelComponent* pCurrentModelComponent = nullptr;
	if (!mRenderable.empty()) {
		pCurrentModelComponent = mRenderable.front();
		mRenderable.pop();
	}

	GLint prevCullMode;
	GLint prevDepthMode;

	glm::mat4 projection = CAMERA_MANAGER->GetCameraProjectionMatrix();
	glm::mat4 view = CAMERA_MANAGER->GetCameraViewMatrix();

	// While there are components to be rendered
	while (pCurrentModelComponent) {
		Transform* pTr = ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform");

		// Get Entity type
		std::string entityType = EntityManager::GetInstance()->GetEntity(pCurrentModelComponent->GetOwnerID())->GetType();
		if (entityType == "Skybox") {
			// Save previous Cull Face and Depth Function Modes
			glGetIntegerv(GL_CULL_FACE_MODE, &prevCullMode);
			glGetIntegerv(GL_DEPTH_FUNC, &prevDepthMode);

			// Set new Cull Face and Depth Function Modes
			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);

			pTr->SetPosition(CAMERA_MANAGER->GetCameraPosition());
		}

		// Set Billboard variables
		glm::vec3 scale = pTr->GetScale();
		mpGBufferShader->SetUniform("scale", scale.x, scale.y, scale.z);
		mpGBufferShader->SetUniform("isBillboard", pCurrentModelComponent->mIsBillboard);

		// Set Model Values
		// Setup MVP Matrix
		// Get Matrices
		glm::mat4 model = pTr->ComputeModelMatrix();

		// Set Matrices
		mpGBufferShader->SetUniform("modelMatrix", model);
		mpGBufferShader->SetUniform("viewMatrix", view);
		mpGBufferShader->SetUniform("projectionMatrix", projection);
		
		// Set Animation Matrices
		glm::mat4 affineAnimationMatrix = glm::mat4(1.0f);
		std::vector<glm::mat4> transformations;
		Animator* pAnimator = ComponentManager::GetInstance()->GetComponent<Animator>(pCurrentModelComponent->GetOwnerID(), "Animator");
		if (pAnimator) {
			pAnimator->GetAffineAnimationMatrix();
			if (pCurrentModelComponent->mpModel->mpSkeleton) {
				transformations = pAnimator->GetSkeletalTransformations(pCurrentModelComponent->mpModel);
			}
		}

		mpGBufferShader->SetUniform("isAnimated", !transformations.empty());

		for (unsigned int i = 0; i < transformations.size(); ++i) {
			std::string uniformName = "boneTransformations[" + std::to_string(i) + "]";
			mpGBufferShader->SetUniform(uniformName, transformations[i]);
		}

		mpGBufferShader->SetUniform("affineAnimationMatrix", affineAnimationMatrix);

		// Enable wireframe
		if (pCurrentModelComponent->mDebugDraw & DRAW_WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Draw 
		std::vector<Mesh*> meshes = pCurrentModelComponent->mpModel->mMeshes;
		for (int i = 0; i < meshes.size(); ++i) {
			// Get Material
			Material* pMaterial = pCurrentModelComponent->mpMaterial;
			if (pCurrentModelComponent->mMeshMaterials[i])
				pMaterial = pCurrentModelComponent->mMeshMaterials[i];

			// Set Material values
			Texture* pDiffuseTexture = pMaterial->mpDiffuseTexture;
			if (pDiffuseTexture) {
				if (entityType == "Skybox") {
					pDiffuseTexture->Bind(mpGBufferShader, "cubeMap", 10);
					mpGBufferShader->SetUniform("UsesCubeMap", true);
				}
				else {
					pDiffuseTexture->Bind(mpGBufferShader, "modelTexture", 0);
					mpGBufferShader->SetUniform("UsesCubeMap", false);
				}
				mpGBufferShader->SetUniform("UsesTexture", true);
			}
			else {
				// Ignore transparent meshes without textures
				if (pMaterial->mDiffuseColor.a < 1.0f)
					continue;

				mpGBufferShader->SetUniform("UsesTexture", false);
			}

			Texture* pNormalMap = pMaterial->mpNormalMap;
			if (pNormalMap) {
				pNormalMap->Bind(mpGBufferShader, "normalMap", 11);
				mpGBufferShader->SetUniform("UsesNormalMap", true);
			}
			else {
				mpGBufferShader->SetUniform("UsesNormalMap", false);
			}

			mpGBufferShader->SetUniform("diffuse", pMaterial->mDiffuseColor.x, pMaterial->mDiffuseColor.y, pMaterial->mDiffuseColor.z, pMaterial->mDiffuseColor.w);
			mpGBufferShader->SetUniform("specular", pMaterial->mSpecularColor.x, pMaterial->mSpecularColor.y, pMaterial->mSpecularColor.z, pMaterial->mShininess);

			mpGBufferShader->SetUniform("UsesLighting", pMaterial->mUseLighting);

			// Draw Mesh
			glBindVertexArray(meshes[i]->mVAO);
			glDrawElements(GL_TRIANGLES, (int)meshes[i]->mIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		
			// Unbind textures
			if (pDiffuseTexture)
				pDiffuseTexture->Unbind();

			if (pNormalMap)
				pNormalMap->Unbind();
		}

		// Disable wireframe
		if (pCurrentModelComponent->mDebugDraw & DRAW_WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Pop next component to be rendered
		if (!mRenderable.empty()) {
			pCurrentModelComponent = mRenderable.front();
			mRenderable.pop();
		}
		else {
			pCurrentModelComponent = nullptr;
		}

		if (entityType == "Skybox") {
			// Restore Cull Face and Depth Function Modes
			glCullFace(prevCullMode);
			glDepthFunc(prevDepthMode);
		}
	}

	mpGBuffer->Unbind();
	mpGBufferShader->Unuse();
}

void Renderer::AmbientRender() 
{
	mpAmbientShader->Use();
	mpPBuffer->Bind();

	glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set GBuffer Textures
	mpGBuffer->BindTexture(mpAmbientShader, "positionTexture", 1, POSITION_INDEX);
	mpGBuffer->BindTexture(mpAmbientShader, "normalTexture", 2, NORMAL_INDEX);
	mpGBuffer->BindTexture(mpAmbientShader, "diffuseTexture", 3, DIFFUSE_INDEX);
	mpGBuffer->BindTexture(mpAmbientShader, "specularTexture", 4, SPECULAR_INDEX);

	// Set Screen Size
	mpAmbientShader->SetUniform("screenSize", float(mpPBuffer->mWidth), float(mpPBuffer->mHeight));

	// Set Model Matrix
	glm::mat4 I(1.0f);
	mpAmbientShader->SetUniform("modelMatrix", glm::scale(I, glm::vec3(2.0f)));

	// Get Mesh
	Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
	// Draw Mesh
	glBindVertexArray(pQuadMesh->mVAO);
	glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	mpGBuffer->UnbindTexture();

	mpPBuffer->Unbind();
	mpAmbientShader->Unuse();
}

void Renderer::GlobalRender() 
{
	// Loop through each global light and compute shadows
	std::vector<Component*> globalLights = ComponentManager::GetInstance()->GetComponents("DRLightComponent");
	for (Component* pGlobalLight : globalLights) {
		// Get light properties
		DRLightComponent* pDirectionalLight = static_cast<DRLightComponent*>(pGlobalLight);

		glm::vec3 lightPos = ComponentManager::GetInstance()->GetComponent<Transform>(pGlobalLight->GetOwnerID(), "Transform")->GetPosition();
		glm::vec3 lightColor = pDirectionalLight->mLcolor;
		glm::vec3 lightDirection = pDirectionalLight->GetDirection();
		glm::mat4 lightView = pDirectionalLight->GetViewMatrix();
		glm::mat4 lightPerspective = pDirectionalLight->GetPerspectiveMatrix();

		ShadowRender(lightView, lightPerspective);

		mpGlobalShader->Use();

		mpPBuffer->Bind();

		glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Compute Shadow Matrix
		glm::mat4 I(1.0f);
		glm::mat4 shadowMatrix = lightPerspective * lightView;

		mpGlobalShader->SetUniform("shadowMatrix", shadowMatrix);

		// Set GBuffer Textures
		mpGBuffer->BindTexture(mpGlobalShader, "positionTexture", 1, POSITION_INDEX);
		mpGBuffer->BindTexture(mpGlobalShader, "normalTexture", 2, NORMAL_INDEX);
		mpGBuffer->BindTexture(mpGlobalShader, "diffuseTexture", 3, DIFFUSE_INDEX);
		mpGBuffer->BindTexture(mpGlobalShader, "specularTexture", 4, SPECULAR_INDEX);

		// Set Shadow Texture
		mpDepthFBO->BindTexture(mpGlobalShader, "shadowTexture", 5, DEPTH_INDEX);

		// Lighting details
		glm::vec3 eyePos = CAMERA_MANAGER->GetCameraPosition();
		mpGlobalShader->SetUniform("eyePos", eyePos.x, eyePos.y, eyePos.z);
		mpGlobalShader->SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
		mpGlobalShader->SetUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
		mpGlobalShader->SetUniform("lightDirection", lightDirection.x, lightDirection.y, lightDirection.z);
		mpGlobalShader->SetUniform("numLights", int(globalLights.size()));

		// Set Screen Size
		mpGlobalShader->SetUniform("screenSize", float(mpPBuffer->mWidth), float(mpPBuffer->mHeight));

		// Set Model Matrix
		I = glm::mat4(1.0f);
		mpGlobalShader->SetUniform("modelMatrix", glm::scale(I, glm::vec3(2.0f)));

		// Get Mesh
		Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
		// Draw Mesh
		glBindVertexArray(pQuadMesh->mVAO);
		glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		mpPBuffer->Unbind();

		mpGlobalShader->Unuse();
	}

	// Loop through each spot light and compute shadows
	std::vector<Component*> spotLights = ComponentManager::GetInstance()->GetComponents("SpotLight");
	for (Component* pSpotLightComponent : spotLights) {
		// Get light properties
		SpotLight* pSpotLight = static_cast<SpotLight*>(pSpotLightComponent);

		glm::vec3 lightPos = ComponentManager::GetInstance()->GetComponent<Transform>(pSpotLight->GetOwnerID(), "Transform")->GetPosition();
		glm::vec3 lightColor = pSpotLight->mColor;
		glm::vec3 lightDirection = pSpotLight->GetDirection();
		glm::mat4 lightView = pSpotLight->GetViewMatrix();
		glm::mat4 lightPerspective = pSpotLight->GetPerspectiveMatrix();

		ShadowRender(lightView, lightPerspective);

		mpSpotShader->Use();

		mpPBuffer->Bind();

		glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Compute Shadow Matrix
		glm::mat4 I(1.0f);
		glm::mat4 shadowMatrix = pSpotLight->GetPerspectiveMatrix() * pSpotLight->GetViewMatrix();

		mpSpotShader->SetUniform("shadowMatrix", shadowMatrix);

		// Set GBuffer Textures
		mpGBuffer->BindTexture(mpSpotShader, "positionTexture", 1, POSITION_INDEX);
		mpGBuffer->BindTexture(mpSpotShader, "normalTexture", 2, NORMAL_INDEX);
		mpGBuffer->BindTexture(mpSpotShader, "diffuseTexture", 3, DIFFUSE_INDEX);
		mpGBuffer->BindTexture(mpSpotShader, "specularTexture", 4, SPECULAR_INDEX);

		// Set Shadow Texture
		mpDepthFBO->BindTexture(mpSpotShader, "shadowTexture", 5, DEPTH_INDEX);

		// Lighting details
		glm::vec3 eyePos = CAMERA_MANAGER->GetCameraPosition();
		mpSpotShader->SetUniform("intensity", pSpotLight->mIntensity);
		mpSpotShader->SetUniform("range", pSpotLight->mRange);
		mpSpotShader->SetUniform("cutoff", cosf(glm::radians(pSpotLight->mSpotAngle)));
		mpSpotShader->SetUniform("eyePos", eyePos.x, eyePos.y, eyePos.z);
		mpSpotShader->SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
		mpSpotShader->SetUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
		mpSpotShader->SetUniform("lightDirection", lightDirection.x, lightDirection.y, lightDirection.z);

		// Set Screen Size
		mpSpotShader->SetUniform("screenSize", float(mpPBuffer->mWidth), float(mpPBuffer->mHeight));

		// Set Model Matrix
		I = glm::mat4(1.0f);
		mpSpotShader->SetUniform("modelMatrix", glm::scale(I, glm::vec3(2.0f)));

		// Get Mesh
		Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
		// Draw Mesh
		glBindVertexArray(pQuadMesh->mVAO);
		glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		mpPBuffer->Unbind();

		mpSpotShader->Unuse();
	}
	
	// Clean Shadow Renderable vector
	mShadowRenderable.clear();
}

void Renderer::LocalRender() 
{
	mpLocalShader->Use();

	mpPBuffer->Bind();

	glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// Enable Front Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	mpLocalShader->SetUniform("viewMatrix", CAMERA_MANAGER->GetCameraViewMatrix());
	mpLocalShader->SetUniform("projectionMatrix", CAMERA_MANAGER->GetCameraProjectionMatrix());

	// Set GBuffer Textures
	mpGBuffer->BindTexture(mpLocalShader, "positionTexture", 1, POSITION_INDEX);
	mpGBuffer->BindTexture(mpLocalShader, "normalTexture", 2, NORMAL_INDEX);
	mpGBuffer->BindTexture(mpLocalShader, "diffuseTexture", 3, DIFFUSE_INDEX);
	mpGBuffer->BindTexture(mpLocalShader, "specularTexture", 4, SPECULAR_INDEX);

	// Set Eye Position
	glm::vec3 eyePos = CAMERA_MANAGER->GetCameraPosition();
	mpLocalShader->SetUniform("eyePos", eyePos.x, eyePos.y, eyePos.z);

	// Set Screen Size
	mpLocalShader->SetUniform("screenSize", float(mpPBuffer->mWidth), float(mpPBuffer->mHeight));


	// Loop through each local light and compute shadows
	std::vector<Component*> localLights = ComponentManager::GetInstance()->GetComponents("LocalLightComponent");
	for (Component* pLocalLight : localLights) {
		// Get light properties
		LocalLightComponent* pLocalLightComponent = static_cast<LocalLightComponent*>(pLocalLight);

		glm::vec3 lightPos = ComponentManager::GetInstance()->GetComponent<Transform>(pLocalLight->GetOwnerID(), "Transform")->GetPosition();

		// Set Model Matrix
		glm::mat4 I(1.0f);
		mpLocalShader->SetUniform("modelMatrix", glm::translate(I, lightPos) * glm::scale(I, glm::vec3(2.0f * pLocalLightComponent->mRadius)));

		// Lighting details
		glm::vec3 lightColor = pLocalLightComponent->mLcolor;
		mpLocalShader->SetUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
		mpLocalShader->SetUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
		mpLocalShader->SetUniform("Radius", pLocalLightComponent->mRadius);
		mpLocalShader->SetUniform("intensity", pLocalLightComponent->mIntensity);
	
		// Get Mesh
		Mesh* pSphereMesh = ResourceManager::GetInstance()->LoadModel("Sphere")->mMeshes[0];
		// Draw Mesh
		glBindVertexArray(pSphereMesh->mVAO);
		glDrawElements(GL_TRIANGLES, (int)pSphereMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glDisable(GL_CULL_FACE);

	mpPBuffer->Unbind();

	mpLocalShader->Unuse();
}

void Renderer::ForwardRender() 
{
	// Copy GBuffer's Depth buffer into the PBuffer's Depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mpGBuffer->GetID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mpPBuffer->GetID());

	glBlitFramebuffer(0, 0, mpGBuffer->mWidth, mpGBuffer->mHeight, 0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	// Sort transparent objects according to distance from camera
	std::sort(mForwardRenderable.begin(), mForwardRenderable.end(), CompareDistance);

	mpForwardShader->Use();
	mpPBuffer->Bind();

	glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// Set Depth Function
	glDepthFunc(GL_LEQUAL);

	// Set Alpha Blend function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set VP Matrix
	glm::mat4 projection = CAMERA_MANAGER->GetCameraProjectionMatrix();
	glm::mat4 view = CAMERA_MANAGER->GetCameraViewMatrix();
	
	mpForwardShader->SetUniform("projectionMatrix", projection);
	mpForwardShader->SetUniform("viewMatrix", view);

	// Render each model from the perspective of the given light
	for (ModelComponent* pCurrentModelComponent : mForwardRenderable) {

		// Setup Model Matrix
		glm::mat4 model = ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform")->ComputeModelMatrix();
		mpForwardShader->SetUniform("modelMatrix", model);

		// Enable wireframe
		if (pCurrentModelComponent->mDebugDraw & DRAW_WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Set Animation Matrices
		glm::mat4 affineAnimationMatrix = glm::mat4(1.0f);
		std::vector<glm::mat4> transformations;
		Animator* pAnimator = ComponentManager::GetInstance()->GetComponent<Animator>(pCurrentModelComponent->GetOwnerID(), "Animator");
		if (pAnimator) {
			pAnimator->GetAffineAnimationMatrix();
			if (pCurrentModelComponent->mpModel->mpSkeleton) {
				transformations = pAnimator->GetSkeletalTransformations(pCurrentModelComponent->mpModel);
			}
		}

		// Set Billboard variables
		Transform* pTr = ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform");
		glm::vec3 scale = pTr->GetScale();
		mpForwardShader->SetUniform("scale", scale.x, scale.y, scale.z);
		mpForwardShader->SetUniform("isBillboard", pCurrentModelComponent->mIsBillboard);

		mpForwardShader->SetUniform("isAnimated", !transformations.empty());

		for (unsigned int i = 0; i < transformations.size(); ++i) {
			std::string uniformName = "boneTransformations[" + std::to_string(i) + "]";
			mpForwardShader->SetUniform(uniformName, transformations[i]);
		}

		mpForwardShader->SetUniform("affineAnimationMatrix", affineAnimationMatrix);

		// Draw 
		std::vector<Mesh*> meshes = pCurrentModelComponent->mpModel->mMeshes;
		for (int i = 0; i < meshes.size(); ++i) {
			// Get Material
			Material* pMaterial = pCurrentModelComponent->mpMaterial;
			if (pCurrentModelComponent->mMeshMaterials[i])
				pMaterial = pCurrentModelComponent->mMeshMaterials[i];

			// Change Blending Mode if necessary
			if (pMaterial->mAdditiveBlending)
				glBlendFunc(GL_ONE, GL_ONE);

			// Ignore opaque meshes
			if ((pMaterial->mDiffuseColor.a >= 1.0f/* || pMaterial->mpDiffuseTexture*/) && !pMaterial->mAdditiveBlending)
				continue;

			// Set Material values
			Texture* pDiffuseTexture = pMaterial->mpDiffuseTexture;
			if (pDiffuseTexture) {
				pDiffuseTexture->Bind(mpForwardShader, "modelTexture", 0);
				mpForwardShader->SetUniform("UsesTexture", true);
			}
			else {
				mpForwardShader->SetUniform("UsesTexture", false);
			}

			mpForwardShader->SetUniform("diffuse", pMaterial->mDiffuseColor.x, pMaterial->mDiffuseColor.y, pMaterial->mDiffuseColor.z, pMaterial->mDiffuseColor.w);

			// Draw Mesh
			glBindVertexArray(meshes[i]->mVAO);
			glDrawElements(GL_TRIANGLES, (int)meshes[i]->mIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			
			if (pDiffuseTexture)
				pDiffuseTexture->Unbind();

			// Revert Changes to Blending Mode
			if (pMaterial->mAdditiveBlending)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		// Disable wireframe
		if (pCurrentModelComponent->mDebugDraw & DRAW_WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	mForwardRenderable.clear();
	

	mpForwardShader->Unuse();

 	ParticleManager::GetInstance()->Draw();
	DebugRender();
	OutlineRender();

	//Text Rendering
	while(!mTextQueue.empty())
	{
		TextInfo text = mTextQueue.front();
		DrawRenderableText(text);
		mTextQueue.pop();
	}

	mpPBuffer->Unbind();

	glBlendFunc(GL_ONE, GL_ONE);
}

void Renderer::RenderText(std::wstring a_text, GLfloat a_x, GLfloat a_y, GLfloat a_scale, float a_r, float a_g, float a_b, float a_a)
{
	TextInfo t = TextInfo();
	float x, y;
	Engine::GetWindowSize(x, y);
	float centerX = x * .5f;
	float centerY = y * .5f;
	t.mText = a_text;
	t.mX = centerX + a_x;
	t.mY = centerY + a_y;
	t.mScale = a_scale;
	t.mR = a_r;
	t.mG = a_g;
	t.mB = a_b;
	t.mA = a_a;
	mTextQueue.push(t);
}

void Renderer::Fade(bool a_fadeIn, float a_fadeTime)
{
	mFade = true;
	mFadeIn = a_fadeIn;
	mFadeTime = 0.0f;
	mFadeMaxTime = a_fadeTime;

	if (a_fadeIn)
		mFadeMultiplier = 0.0f;
	else
		mFadeMultiplier = 1.0f;
}

float Renderer::GetFadeMultiplier()
{
	return mFadeMultiplier;
}

GEAR_API void Renderer::RenderContacts()
{
	std::list<Contact*> contacts = PHYSICS_MANAGER->GetCollisionManager()->mActualContacts;

	// Setup Model Matrix
	for (Contact* con : contacts) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), con->colPoint) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		mpLineShader->SetUniform("modelMatrix", modelMatrix);

		// Setup Color (Black)
		mpLineShader->SetUniform("diffuse", 1.0f, 0.5f, 1.0f, 1.0f);
		// Setup Velocity vertices
		GLfloat velVertices[] = {
			0.0f, 0.0f, 0.0f,
			con->mNormal.x, con->mNormal.y, con->mNormal.z
		};

		glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(velVertices), &velVertices, GL_STATIC_DRAW);

		glDrawArrays(GL_LINES, 0, 2);

		glEnable(GL_DEPTH_TEST);
	}
}

void Renderer::DebugRender() 
{
	mpSilhouetteBuffer->Bind();

	// Set clear color of silhouette buffer to white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	mpSilhouetteBuffer->Unbind();


	mpPBuffer->Bind();

	mpLineShader->Use();

	// Set VP Matrix
	glm::mat4 projection = CAMERA_MANAGER->GetCameraProjectionMatrix();
	glm::mat4 view = CAMERA_MANAGER->GetCameraViewMatrix();

	mpLineShader->SetUniform("projectionMatrix", projection);
	mpLineShader->SetUniform("viewMatrix", view);

	// Bind VAO used for debug draw vertices
	glBindVertexArray(mDebugVAO);

	// For each model in the debug renderable list, draw the appropriate debug types
	for (ModelComponent* pModelComponent : mDebugRenderable) {
		if (pModelComponent->mDebugDraw & DRAW_AABB)
			DrawAABB(pModelComponent);

		if (pModelComponent->mDebugDraw & DRAW_VELOCITY)
			DrawVelocity(pModelComponent);

		if (pModelComponent->mDebugDraw & DRAW_COLLIDER)
			DrawCollider(pModelComponent);
	}
	mDebugRenderable.clear();
	if (LEVEL_EDITOR->showTree)
		DrawAABBTree(PHYSICS_MANAGER->GetTree());
	if (LEVEL_EDITOR->showContacts)
		RenderContacts();
	glBindVertexArray(0);

	mpLineShader->Unuse();
}

void Renderer::OutlineRender()
{
	// Copy GBuffer's Depth buffer into the PBuffer's Depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mpPBuffer->GetID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mpSilhouetteBuffer->GetID());

	glBlitFramebuffer(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight, 0, 0, mpSilhouetteBuffer->mWidth, mpSilhouetteBuffer->mHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	mpPBuffer->Bind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// For each model in the debug renderable list, draw the appropriate debug types
	while (!mOutlineQueue.empty()) {
		Outline outlineDetails = mOutlineQueue.front();

		if (!outlineDetails.respectOcclusion) {
			// Disable Depth Test so that the outline is drawn over the object
			glDisable(GL_DEPTH_TEST);
		}

		mpLineShader->Use();

		mpLineShader->SetUniform("diffuse", outlineDetails.color.r, outlineDetails.color.g, outlineDetails.color.b, 1.0f);

		// Set VP Matrix
		glm::mat4 projection = CAMERA_MANAGER->GetCameraProjectionMatrix();
		glm::mat4 view = CAMERA_MANAGER->GetCameraViewMatrix();

		mpLineShader->SetUniform("projectionMatrix", projection);
		mpLineShader->SetUniform("viewMatrix", view);

		// Copy GBuffer's Depth buffer into the PBuffer's Depth buffer
		mpSilhouetteBuffer->Bind();
		glViewport(0, 0, mpSilhouetteBuffer->mWidth, mpSilhouetteBuffer->mHeight);

		// Set clear color of silhouette buffer to white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw Silhouette of sphere collider

		// Setup Model Matrix
		Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(outlineDetails.pModelComponent->GetOwnerID(), "Transform");
		glm::vec3 scale = pTr->GetScale();
		pTr->SetScale(1.05f * scale);
		glm::mat4 modelMatrix = pTr->ComputeModelMatrix();
		mpLineShader->SetUniform("modelMatrix", modelMatrix);
		pTr->SetScale(scale);

		std::vector<Mesh*> meshes = outlineDetails.pModelComponent->mpModel->mMeshes;
		for (Mesh* pMesh : meshes) {
			// Draw Mesh
			glBindVertexArray(pMesh->mVAO);
			glDrawElements(GL_TRIANGLES, (int)pMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		mpSilhouetteBuffer->Unbind();


		// Rebind PBuffer
		mpPBuffer->Bind();
		glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);

		// Draw Outline
		mpOutlineShader->Use();

		// Set GBuffer Textures
		mpSilhouetteBuffer->BindTexture(mpOutlineShader, "silhouetteTexture", 1, COLOR_INDEX);

		// Set Screen Size
		mpOutlineShader->SetUniform("screenSize", float(mpPBuffer->mWidth), float(mpPBuffer->mHeight));

		// Set Model Matrix
		glm::mat4 I(1.0f);
		mpOutlineShader->SetUniform("modelMatrix", glm::scale(I, glm::vec3(2.0f)));

		// Set width of outline
		mpOutlineShader->SetUniform("w", 4);

		// Get Mesh
		Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
		// Draw Mesh
		glBindVertexArray(pQuadMesh->mVAO);
		glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(mDebugVAO);

		mpSilhouetteBuffer->UnbindTexture();

		mpOutlineShader->Unuse();

		if (!outlineDetails.respectOcclusion) {
			// Disable Depth Test so that the outline is drawn over the object
			glEnable(GL_DEPTH_TEST);
		}

		mOutlineQueue.pop();
	}
}

void Renderer::PBufferRender() 
{
	mpPBufferShader->Use();
	float w, h;
	Engine::GetWindowSize(w, h);
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set GBuffer Textures
	mpPBuffer->BindTexture(mpPBufferShader, "colorTexture", 1, COLOR_INDEX);

	// Set Screen Size
	mpPBufferShader->SetUniform("screenSize", w, h);

	// Set Anti-Aliasing Preference
	mpPBufferShader->SetUniform("antiAliasing", mAntiAliasing);

	mpPBufferShader->SetUniform("spanMax", mSpanMax);
	mpPBufferShader->SetUniform("reduceMin", mReduceMin);
	mpPBufferShader->SetUniform("reduceMultiplier", mReduceMultiplier);

	// Update Fade
	if (mFade) {
		mFadeTime += float(FRAMERATE_MANAGER->GetFrameTime()) * 0.001f;
		float t = mFadeTime / mFadeMaxTime;

		if (t > 1.0f) {
			mFade = false;
			t = 1.0f;
		}

		float to = 0.0f;
		float from = 1.0f;
		if (mFadeIn) {
			to = 1.0f;
			from = 0.0f;
		}

		mFadeMultiplier = glm::lerp(from, to, t);;
	}

	// Set Fade Parameters
	mpPBufferShader->SetUniform("fadeMultiplier", mFadeMultiplier);
	
	// Set Model Matrix
	glm::mat4 I(1.0f);
	mpPBufferShader->SetUniform("modelMatrix", glm::scale(I, glm::vec3(2.0f)));

	// Get Mesh
	Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
	// Draw Mesh
	glBindVertexArray(pQuadMesh->mVAO);
	glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	mpPBuffer->UnbindTexture();

	mpPBufferShader->Unuse();
}

void Renderer::UIRender()
{
	// Pop first component to be rendered
	ModelComponent* pCurrentModelComponent = nullptr;
	if (!mUIQueue.empty()) {
		pCurrentModelComponent = mUIQueue.front();
		mUIQueue.pop();
	}

	// While there are components to be rendered
	while (pCurrentModelComponent) {
		Transform* pTr = ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform");
		UIComponent* pUI = ComponentManager::GetInstance()->GetComponent<UIComponent>(pCurrentModelComponent->GetOwnerID(), "UIComponent");
		Material* pMaterial = pCurrentModelComponent->mpMaterial;
		if (pMaterial->mpDiffuseTexture) {
			glm::vec3 pos = pTr->GetPosition();
			glm::vec3 scl = pTr->GetScale();
			if (pUI->mData.sType == IMAGE)
			{
				glm::vec3 position = pos;
				position.x *= Engine::GetInstance()->mWindowWidth;
				position.y *= Engine::GetInstance()->mWindowHeight;
				if (pUI->mData.sImageDescription == "Slider")
				{
					glm::vec3 scale = scl;
					scale.x *= pUI->mScaleFactor;
					position.x += scale.x / 2;
					pTr->SetScale(scale);
				}
				pTr->SetPosition(position);
				
				//ImGui::Image((void*)(intptr_t)pMaterial->mpDiffuseTexture->mTextureID, ImVec2(scale.x, scale.y));
				mpDefaultShader->Use();
				glm::mat4 model = ComponentManager::GetInstance()->GetComponent<Transform>(pCurrentModelComponent->GetOwnerID(), "Transform")->ComputeModelMatrix();
				glm::mat4 projection = CAMERA_MANAGER->GetCameraOrthogonalMatrix();

				pTr->SetPosition(pos);
				if (pUI->mData.sImageDescription == "Slider")
				{
					pTr->SetScale(scl);
				}
				
				mpDefaultShader->SetUniform("modelMatrix", model);
				mpDefaultShader->SetUniform("projectionMatrix", projection);
				mpDefaultShader->SetUniform("diffuseTexture", 0);

				glActiveTexture((GLenum)((int)GL_TEXTURE0));
				glBindTexture(GL_TEXTURE_2D, pMaterial->mpDiffuseTexture->mTextureID);

				Mesh* pQuadMesh = ResourceManager::GetInstance()->LoadModel("Quad")->mMeshes[0];
				// Draw Mesh
				glBindVertexArray(pQuadMesh->mVAO);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				//glDisable(GL_DEPTH_TEST);
				glDrawElements(GL_TRIANGLES, (int)pQuadMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
				glDisable(GL_BLEND);
				//glEnable(GL_DEPTH_TEST);
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
				mpDefaultShader->Unuse();
			}
			
			else
			{
				ImGui::SetNextWindowPos(ImVec2(Engine::GetInstance()->mWindowWidth * pos.x, Engine::GetInstance()->mWindowHeight * pos.y));
				ImGui::SetNextWindowSize(ImVec2(scl.x, scl.y));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::Begin(std::to_string(pCurrentModelComponent->GetOwnerID()).c_str(), (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			
				if (pUI->mData.sType == BUTTON)
				{
					
					if (pUI->mIsLevelButton) {
					//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(255.0f,0.0f,0.0f,0.5f));
						if (!pUI->mIsUnlocked)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
						bool temp = ImGui::Button(pUI->mData.sButtonLabel.c_str(), ImVec2(scl.x, scl.y));
					//	ImGui::PopStyleColor();
						if (ImGui::IsItemHovered())
						{
							//ImGui::ColorButton(pUI->mData.sButtonLabel.c_str(), ImVec4(200,200,200,0.5));
							glm::vec3 position = pTr->GetPosition();
							ImGui::SetNextWindowPos(ImVec2(Engine::GetInstance()->mWindowWidth * (position.x + 0.11), Engine::GetInstance()->mWindowHeight * position.y));
							ImGui::SetNextWindowSize(ImVec2(scl.x - 50.0f, scl.y - 30.0f ));
							ImGui::Begin("TEST", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
							ImGui::SetWindowFontScale(1.1);
							ImGui::TextWrapped("High Score : %d", pUI->mHighScore);
							ImGui::End();
						}
						if (temp)
						{
							UIActionEvent* pUIEvent = static_cast<UIActionEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UIActionEvent"));
							pUIEvent->mDescription = pUI->mData.sButtonLabel;
							pUIEvent->mOwnerId = pUI->GetOwnerID();
							EVENT_MANAGER->BroadcastEvent(pUIEvent);


						}
						if (!pUI->mIsUnlocked)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
					}
					else
					{
						bool temp = ImGui::Button(pUI->mData.sButtonLabel.c_str(), ImVec2(scl.x, scl.y));

						if (temp)
						{
							UIActionEvent* pUIEvent = static_cast<UIActionEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UIActionEvent"));
							pUIEvent->mDescription = pUI->mData.sButtonLabel;
							pUIEvent->mOwnerId = pUI->GetOwnerID();
							EVENT_MANAGER->BroadcastEvent(pUIEvent);


						}
					}
					
				}
				else if (pUI->mData.sType == RADIOBUTTON)
				{
					for (int i = 0; i < pUI->mData.sRadioButtonOptions.size(); ++i)
					{
						ImGui::SameLine();
						if (ImGui::RadioButton(pUI->mData.sRadioButtonOptions[i].c_str(), &pUI->mData.sRadioButtonSelected, i))
						{
							UIActionEvent* pUIEvent = static_cast<UIActionEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UIActionEvent"));
							pUIEvent->mData =  pUI->mData.sRadioButtonOptions[i];
							pUIEvent->mDescription = pUI->mData.sRadioButtonLabel;
							pUIEvent->mOwnerId = pUI->GetOwnerID();
							EVENT_MANAGER->BroadcastEvent(pUIEvent);
						}
					}
				}
				else if (pUI->mData.sType == SLIDER)
				{
					if (ImGui::SliderInt(pUI->mData.sSliderLabel.c_str(), &pUI->mData.sSliderValue, pUI->mData.sMinNum, pUI->mData.sMaxNum))
					{
						UIActionEvent* pUIEvent = static_cast<UIActionEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UIActionEvent"));
						pUIEvent->mDescription = pUI->mData.sSliderLabel;
						pUIEvent->mOwnerId = pUI->GetOwnerID();
						EVENT_MANAGER->BroadcastEvent(pUIEvent);
					}
				}
				else if (pUI->mData.sType == TEXT)
				{
					ImGui::Text(pUI->mData.sTextString.c_str());
				}
				ImGui::End();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
		}

		// Pop next component to be rendered
		if (!mUIQueue.empty()) {
			pCurrentModelComponent = mUIQueue.front();
			mUIQueue.pop();
		}
		else {
			pCurrentModelComponent = nullptr;
		}
	}
}
std::vector<glm::vec3> colors = {
	glm::vec3(1),
	glm::vec3(0.5,1,1),
	glm::vec3(0,1,1),
	glm::vec3(1,0.5,1),
	glm::vec3(1,0,1),
	glm::vec3(1,1,0.5),
	glm::vec3(1,1,0),
	glm::vec3(1,0,0),
	glm::vec3(0,1,0), 
	glm::vec3(0,0,1),
	glm::vec3(0.5,0,0),
	glm::vec3(0,0.5,0),
	glm::vec3(0,0,0.5),
	glm::vec3(0) };
GEAR_API void Renderer::DrawAABBTree(AABBTree * tree, glm::vec3 a_color)
{
	std::vector<Node*> treeNodes = tree->GetNodes();
	for (Node* n : treeNodes) {
		if (n->height == -1) continue;
		glm::vec3 max = n->box->mCenter + n->box->mE;
		glm::vec3 min = n->box->mCenter - n->box->mE;
		glm::vec2 minMaxX = glm::vec2(min.x, max.x);
		glm::vec2 minMaxY = glm::vec2(min.y, max.y);
		glm::vec2 minMaxZ = glm::vec2(min.z, max.z); ;

		mpLineShader->SetUniform("modelMatrix", glm::mat4(1.0f));
		// Setup Color (Black)
		glm::vec3 color = colors[n->height];
		mpLineShader->SetUniform("diffuse", color.x, color.y, color.z, 1.0f);

		DrawBox(minMaxX, minMaxY, minMaxZ);
	}
}

void Renderer::DrawAABB(ModelComponent* a_pModelComponent, glm::vec3 a_color)
{
	glm::vec2 minMaxX = a_pModelComponent->mpModel->mMinMaxX;
	glm::vec2 minMaxY = a_pModelComponent->mpModel->mMinMaxY;
	glm::vec2 minMaxZ = a_pModelComponent->mpModel->mMinMaxZ;

	// Setup Model Matrix
	glm::mat4 modelMatrix = ComponentManager::GetInstance()->GetComponent<Transform>(a_pModelComponent->GetOwnerID(), "Transform")->ComputeModelMatrix();
	mpLineShader->SetUniform("modelMatrix", modelMatrix);
	// NOTE: This box is not axis aligned if it rotates

	// Setup Color (Black)
	mpLineShader->SetUniform("diffuse", 0.0f, 0.0f, 0.0f, 1.0f);

	DrawBox(minMaxX, minMaxY, minMaxZ);
}

void Renderer::DrawVelocity(ModelComponent* a_pModelComponent) 
{
	Body* pBody = COMPONENT_MANAGER->GetComponent<Body>(a_pModelComponent->GetOwnerID(), "Body");
	
	// Don't continue if the current entity doesn't have a body
	if (!pBody)
		return;

	// Disable Depth Test so that the velocity is drawn over the object
	glDisable(GL_DEPTH_TEST);

	// Setup Model Matrix
	Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(a_pModelComponent->GetOwnerID(), "Transform");
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), pTr->GetPosition()) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	mpLineShader->SetUniform("modelMatrix", modelMatrix);

	// Setup Color (Black)
	mpLineShader->SetUniform("diffuse", 1.0f, 1.0f, 0.0f, 1.0f);
	
	// Setup Velocity vertices
	GLfloat velVertices[] = {
		0.0f, 0.0f, 0.0f,
		pBody->mVel.x, pBody->mVel.y, pBody->mVel.z
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(velVertices), &velVertices, GL_STATIC_DRAW);

	glDrawArrays(GL_LINES, 0, 2);

	mpLineShader->SetUniform("modelMatrix", modelMatrix);

	// Setup Color (Black)
	mpLineShader->SetUniform("diffuse", 0.5f, 0.0f, 1.0f, 1.0f);

	// Setup Angular Velocity vertices
	velVertices[3] = pBody->mOmega.x; 
	velVertices[4] = pBody->mOmega.y; 
	velVertices[5] = pBody->mOmega.z;

	glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(velVertices), &velVertices, GL_STATIC_DRAW);

	glDrawArrays(GL_LINES, 0, 2);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawCollider(ModelComponent* a_pModelComponent)
{
	Collider* pCollider = COMPONENT_MANAGER->GetComponent<Collider>(a_pModelComponent->GetOwnerID(), "Collider");

	// Don't continue if the current entity doesn't have a body
	if (!pCollider)
		return;

	// Disable Depth Test so that the collider is drawn over the object
	glDisable(GL_DEPTH_TEST);

	// Set color of collider line
	if (pCollider->mIsCollided) {
		// Setup Color (Red)
		mpLineShader->SetUniform("diffuse", 1.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		// Setup Color (Green)
		mpLineShader->SetUniform("diffuse", 0.0f, 1.0f, 0.0f, 1.0f);
	}
	glm::vec2 minMaxX;
	glm::vec2 minMaxY;
	glm::vec2 minMaxZ;
	if (pCollider->mpShape->mType == _AABB) {
		AABB* pCube = static_cast<AABB*>(pCollider->mpShape);

		glm::vec3 position =pCube->mCenter;

		minMaxX = glm::vec2( pCube->mLeft, pCube->mRight);
		minMaxY = glm::vec2(pCube->mBottom, pCube->mTop);
		minMaxZ = glm::vec2(pCube->mBack, pCube->mFront);

		// Setup Model Matrix
		mpLineShader->SetUniform("modelMatrix", glm::mat4(1.0f));

		DrawBox(minMaxX, minMaxY, minMaxZ);
	}
	else if (pCollider->mpShape->mType == _SPHERE) {
		SPHERE* pSphere = static_cast<SPHERE*>(pCollider->mpShape);

		// Setup Model Matrix
		glm::vec3 position = ComponentManager::GetInstance()->GetComponent<Transform>(a_pModelComponent->GetOwnerID(), "Transform")->GetPosition();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * pSphere->mRadius));
		mpLineShader->SetUniform("modelMatrix", modelMatrix);

		// Get Mesh
		Mesh* pSphereMesh = ResourceManager::GetInstance()->LoadModel("Sphere")->mMeshes[0];
		// Draw Mesh

		mpLineShader->Use();
		glBindVertexArray(pSphereMesh->mVAO);
		glDrawElements(GL_LINE_LOOP, (int)pSphereMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		mpLineShader->Unuse();
	}
	else if (pCollider->mpShape->mType == _OBB) {
		OBB* pOBB = static_cast<OBB*>(pCollider->mpShape);

		glm::vec3 position = pOBB->mCenter;
		// Setup Model Matrix
		glm::mat4 rot = glm::mat4(pOBB->mRot);
		rot[3][3] = 1;
		glm::mat4 model = glm::translate(glm::mat4(1), position) * rot;
		
		glm::vec3 mins =  - pOBB->mE;
		glm::vec3 maxs =  pOBB->mE;
		minMaxX = glm::vec2(mins.x, maxs.x);
		minMaxY = glm::vec2(mins.y, maxs.y);
		minMaxZ = glm::vec2(mins.z, maxs.z);
		mpLineShader->SetUniform("modelMatrix", model);
		DrawBox(minMaxX, minMaxY, minMaxZ);

	}
	else if (pCollider->mpShape->mType == _CAPSULE) {
		CAPSULE* pCap = static_cast<CAPSULE*>(pCollider->mpShape);

		// Copy GBuffer's Depth buffer into the PBuffer's Depth buffer
		// Rebind PBuffer
		mpPBuffer->Bind();
		glViewport(0, 0, mpPBuffer->mWidth, mpPBuffer->mHeight);

		// Draw Silhouette of sphere collider

		// Setup Model Matrix
		glm::vec3 position = ComponentManager::GetInstance()->GetComponent<Transform>(a_pModelComponent->GetOwnerID(), "Transform")->GetPosition();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), pCap->mCenter) * 
			glm::scale(glm::mat4(1.0f),glm::vec3(2.0f * pCap->mRadius,pCap->mLength+2.f*pCap->mRadius, 2.0f * pCap->mRadius));//check this
		mpLineShader->SetUniform("modelMatrix", modelMatrix);

		// Get Mesh
		Mesh* mesh = ResourceManager::GetInstance()->LoadModel("capsule")->mMeshes[0];
		if (nullptr == mesh) return;//just in case
		// Draw Mesh
		// Reuse Debug Shader
		mpLineShader->Use();
		glBindVertexArray(mesh->mVAO);
		glDrawElements(GL_LINE_LOOP, (int)mesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		mpLineShader->Unuse();
	}
	else if (pCollider->mpShape->mType == _CYLINDER) {
		CYLINDER* pCyl = static_cast<CYLINDER*>(pCollider->mpShape);
		glm::mat4 rot = glm::mat4(0);
		for (int i = 0; i < 3; ++i)
			rot[i] = glm::vec4(pCyl->mRot[i],0);
		rot[3][3] = 1;
		// Setup Model Matrix
		glm::vec3 position = pCyl->mCenter;
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * 
			rot *
			glm::scale(glm::mat4(1.0f), glm::vec3(2.0f * pCyl->mRadius,pCyl->mLength, 2.0f * pCyl->mRadius));
		mpLineShader->SetUniform("modelMatrix", modelMatrix);

		// Get Mesh
		Mesh* pCylMesh = ResourceManager::GetInstance()->LoadModel("Cylinder")->mMeshes[0];
		// Draw Mesh

		mpLineShader->Use();
		glBindVertexArray(pCylMesh->mVAO);
		glDrawElements(GL_LINE_LOOP, (int)pCylMesh->mIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		mpLineShader->Unuse();
	}
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawRenderableText(TextInfo a_text)
{

	glm::vec4 color = glm::vec4(a_text.mR, a_text.mG, a_text.mB, a_text.mA);
	mpTextShader->Use();
	float w, h;
	Engine::GetWindowSize(w, h);
	mpTextShader->SetUniform("textColor", color.x, color.y, color.z, color.w);
	mpTextShader->SetUniform("projection", glm::ortho(0.0f,w, 0.0f, h) );
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(mTextVAO);
	// Iterate through all characters
	std::wstring text = a_text.mText;
	std::wstring::const_iterator c;
	float max_x = 0;
	for (int i = 0; i < text.size(); ++i) // MAKE SURE TO SKIP UNICODE BOM
	{
		wchar_t c = text[i];
		if (mCharacters.find(c) == mCharacters.end())
			LoadCharacter(c);
		Character ch = mCharacters[c];

		max_x += ch.mSize.x  * a_text.mScale;

	}

	float x_start = a_text.mX -  max_x/2.f;

	for (int i = 0; i < text.size(); ++i) // MAKE SURE TO SKIP UNICODE BOM
	{
		wchar_t c = text[i];
		Character ch = mCharacters[c];

		float xpos = x_start + ch.mBearing.x * a_text.mScale;
		float ypos = a_text.mY - (ch.mSize.y - ch.mBearing.y) *  a_text.mScale;

		float w = ch.mSize.x *  a_text.mScale;
		float h = ch.mSize.y *  a_text.mScale;
		// Update VBO for each character
		GLfloat vertices[] = {
			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos,     ypos,       0.0f, 1.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,

			 xpos,     ypos + h,   0.0f, 0.0f ,
			 xpos + w, ypos,       1.0f, 1.0f ,
			 xpos + w, ypos + h,   1.0f, 0.0f
		};
		glBindTexture(GL_TEXTURE_2D, ch.mTextureID);
		mpTextShader->SetUniform("text", 0);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mTextVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x_start += (ch.mAdvance >> 6) * a_text.mScale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

glm::mat4 Renderer::ComputeLightView(glm::vec3 a_lightPosition, glm::vec3 a_lightDirection) 
{
	// Compute light up vector
	glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::cross(worldUp, -1.0f * a_lightPosition);
	glm::vec3 up = glm::cross(-1.0f * a_lightPosition, right);

	// Compute view matrix
	return glm::lookAt(a_lightPosition, a_lightPosition + a_lightDirection, up);
}

void Renderer::DrawBox(glm::vec2 a_minMaxX, glm::vec2 a_minMaxY, glm::vec2 a_minMaxZ ,glm::vec3 color)
{
	// Setup AABB vertices
	GLfloat aabbVertices[] = {
		a_minMaxX.x, a_minMaxY.x, a_minMaxZ.x,
		a_minMaxX.y, a_minMaxY.x, a_minMaxZ.x,
		a_minMaxX.y, a_minMaxY.y, a_minMaxZ.x,
		a_minMaxX.x, a_minMaxY.y, a_minMaxZ.x,
		a_minMaxX.x, a_minMaxY.x, a_minMaxZ.y,
		a_minMaxX.y, a_minMaxY.x, a_minMaxZ.y,
		a_minMaxX.y, a_minMaxY.y, a_minMaxZ.y,
		a_minMaxX.x, a_minMaxY.y, a_minMaxZ.y,
	};

	glBindBuffer(GL_ARRAY_BUFFER, mDebugVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(aabbVertices), &aabbVertices, GL_STATIC_DRAW);

	// Setup AABB indices
	GLuint indices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDebugEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	// Draw AABB
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(4 * sizeof(GLuint)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(8 * sizeof(GLuint)));

	// Unbind Buffers
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Renderer::LoadCharacter(wchar_t a_char)
{
	// Load character glyph 
	if (FT_Load_Char(face, a_char, FT_LOAD_RENDER))
	{
		LOG_WARN("ERROR::FREETYTPE: Failed to load Glyph");
		return;
	}

		// Generate texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
		face->glyph->bitmap.rows, 0, GL_RED,
		GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
	);
		// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
	Character character = {
		texture,
		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			unsigned int(face->glyph->advance.x)
	};
	mCharacters[a_char] = character;
}

void Renderer::ReloadFonts(std::string a_font)
{
	mCharacters.clear();
	
	if (FT_New_Face(ft, a_font.c_str(), 0, &face))//find a way to 
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Int num_maps = face->num_charmaps;
	FT_CharMap found = NULL;
	for (FT_Int n = 0; n < face->num_charmaps; ++n)
	{
		FT_CharMap charmap = face->charmaps[n];
		if (charmap->encoding == FT_ENCODING_UNICODE)
		{
			found = charmap;
			break;
		}
	}

	FT_Set_Charmap(face, found);
	FT_Int num_chars = 0;
	// Load characters found in the face for the utf16 map
	

	glBindTexture(GL_TEXTURE_2D, 0);

}