#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "MaterialEditor.h"
#include "Engine.h"
#include "Managers/ResourceManager.h"
#include "Managers/InputManager.h"
#include "Resources/Material.h"
#include "Resources/Shader.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "FBO.h"
#include "Mesh.h"
#include "Utility.h"

MaterialEditor::MaterialEditor()
{
	mSelectedMaterial = "";
	mEditActive = false;
	mAddPopup = false;
	mDragging = false;
	mpDemoFBO = new FBO(512, 512, 1, false);
	mpDemoShader = RESOURCE_MANAGER->LoadShader("Demo");

	// Setup MVP Matrix
	mDemoPerspective = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	mDemoView = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mDemoModel = glm::toMat4(glm::quat(glm::vec3(-90.0f, 90.0f, 0.0f) * (glm::pi<float>() / 180.0f)));
}

MaterialEditor::~MaterialEditor()
{
	delete mpDemoFBO;
}

void MaterialEditor::Draw(bool& a_active)
{
	DrawMaterialList(a_active);

	DrawMaterialEditor();
}

void MaterialEditor::DrawMaterialList(bool& a_active)
{
	// Setup Window
	float w, h;
	Engine::GetWindowSize(w, h);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f + (h / 2)));
	ImGui::SetNextWindowSize(ImVec2(w / 7, h / 2 - 100));
	ImGui::GetStyle().DisplaySafeAreaPadding.x = 0.0f;

	ImGui::Begin("Material List", &a_active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

	std::vector<std::string> materialFiles = GetUniqueDirectoryFiles(".\\Resources\\Materials\\");
	char id[32];
	for (int i = 0; i < materialFiles.size(); ++i) {
		std::string materialName = materialFiles[i];
		ImGui::PushID(i);
		sprintf(id, "%-6d", i);

		if (ImGui::Selectable(id, materialName == mSelectedMaterial, ImGuiSelectableFlags_AllowDoubleClick)) {
			mSelectedMaterial = materialName;
			if (ImGui::IsMouseDoubleClicked(0))
				mEditActive = true;
		}
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
			LOG_INFO(materialName);
			ImGui::SetDragDropPayload("MatName", &materialName, materialName.size());
			ImGui::Text(std::string("Apply " + materialName + " Material").c_str());
			mDragging = true;
			ImGui::EndDragDropSource();
		}
		if (mDragging && Mouse::GetInstance()->IsReleased(0)) {
			LOG_INFO("DROP!");
			mDragging = false;
			if (!ImGui::IsAnyWindowHovered()) {
				const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("MatName");
				if (pPayload) {
					std::string matName = *(static_cast<std::string*>(pPayload->Data));
					// Ray cast and apply material to mesh
				}
			}
		}
		ImGui::OpenPopupOnItemClick("Matrial Context Menu", 1);
		if (ImGui::BeginPopupContextItem("Material Context Menu")) {
			mSelectedMaterial = materialName;
			if (ImGui::Selectable("Add")) {
				mAddPopup = true;
			}
			// TODO: Implement the rest
			if (ImGui::Selectable("Delete")) {
				mEditActive = false;
			}

			if (ImGui::Selectable("Edit")) {
				mEditActive = true;
			}

			if (ImGui::Selectable("Rename")) {
				
			}

			ImGui::Selectable("Copy");
			ImGui::Selectable("Paste");
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		ImGui::Text(materialName.c_str());
	
		if (mAddPopup) {
			mAddPopup = false;
			ImGui::OpenPopup("Add New");
		}

		ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
		ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
		if (ImGui::BeginPopupModal("Add New", nullptr, 0)) {
			static char inputBuffer[64] = "";
			ImGui::Text("Enter file name:");
			if (ImGui::InputText("", inputBuffer, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
				mSelectedMaterial = std::string(inputBuffer);
				SaveMaterial(RESOURCE_MANAGER->LoadMaterial("Default"));
				memset(inputBuffer, 0, 64);
				ImGui::CloseCurrentPopup();

			}
			else if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !ImGui::IsItemHovered()) {
				memset(inputBuffer, 0, 64);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	ImGui::End();
}

void MaterialEditor::DrawMaterialEditor()
{
	if (mEditActive) {
		// Setup Window
		float w, h;
		Engine::GetWindowSize(w, h);
		ImGui::SetNextWindowPos(ImVec2(6.0f * w / 7.0f, 100.0f));
		ImGui::SetNextWindowSize(ImVec2(w / 7, h - 100));

		ImGui::Begin(std::string("Material Editor - " + mSelectedMaterial + ".mat###MtrlEdtr").c_str(), &mEditActive, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);

		Material* pMaterial = RESOURCE_MANAGER->LoadMaterial(mSelectedMaterial);

		if (pMaterial) {
			// Draw Material
			DrawDemoMaterial(pMaterial);

			ImGui::BeginChild("Material Properties", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

			// Select Texture
			std::vector<std::string> textureNames = GetUniqueDirectoryFiles(".\\Resources\\Textures\\");
			textureNames.push_back("None");
			std::string currentTextureName = "None";
			if (pMaterial->mpDiffuseTexture)
				currentTextureName = pMaterial->mpDiffuseTexture->mName;
			if (ImGui::BeginCombo(std::string("Texture##Texture").c_str(), currentTextureName.c_str())) {
				for (std::string textureName : textureNames) {
					bool isSelected = (currentTextureName == textureName);
					if (ImGui::Selectable(textureName.c_str(), isSelected)) {
						if (textureName == "None")
							pMaterial->mpDiffuseTexture = nullptr;
						else
							pMaterial->mpDiffuseTexture = RESOURCE_MANAGER->LoadTexture(textureName);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Select Normal Map
			std::string currentNormalMapName = "None";
			if (pMaterial->mpNormalMap)
				currentNormalMapName = pMaterial->mpNormalMap->mName;
			if (ImGui::BeginCombo(std::string("Normal Map##NormalMap").c_str(), currentNormalMapName.c_str())) {
				for (std::string textureName : textureNames) {
					bool isSelected = (currentNormalMapName == textureName);
					if (ImGui::Selectable(textureName.c_str(), isSelected)) {
						if (textureName == "None")
							pMaterial->mpNormalMap = nullptr;
						else
							pMaterial->mpNormalMap = RESOURCE_MANAGER->LoadTexture(textureName);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Select Diffuse Color
			ImGui::ColorEdit4("Diffuse", &pMaterial->mDiffuseColor[0]);

			// Select Specular Color
			ImGui::ColorEdit3("Specular", &pMaterial->mSpecularColor[0]);

			// Select Shininess
			ImGui::DragFloat("Shininess", &pMaterial->mShininess, 1.0f, 1.0f, 512.0f, "%.1f");

			// Select Shader
			std::vector<std::string> shaderNames = GetUniqueDirectoryFiles(".\\Resources\\Shaders\\");
			shaderNames.push_back("None");
			std::string currentShaderName = "None";
			if (pMaterial->mpShader)
				currentShaderName = pMaterial->mpShader->mName;
			if (ImGui::BeginCombo(std::string("Shader##Shader").c_str(), currentShaderName.c_str())) {
				for (std::string shaderName : shaderNames) {
					bool isSelected = (currentShaderName == shaderName);
					if (ImGui::Selectable(shaderName.c_str(), isSelected)) {
						if (shaderName == "None")
							pMaterial->mpShader = nullptr;
						else
							pMaterial->mpShader = RESOURCE_MANAGER->LoadShader(shaderName);
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// Toggle AdditiveBlending
			ImGui::Checkbox("Additive Blending", &pMaterial->mAdditiveBlending);

			// Toggle Lighting
			ImGui::Checkbox("Lighting", &pMaterial->mUseLighting);

			ImGui::EndChild();


			ImVec2 buttonSize(60, 20);
			ImGui::BeginChild("Control Buttons", ImVec2(0, buttonSize.y), false, 0);

			// Close
			if (ImGui::Button("Close", buttonSize)) {
				mEditActive = false;
			}
			ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 2.0f * buttonSize.x - ImGui::GetStyle().ItemSpacing.x);
			// Save
			if (ImGui::Button("Save As", buttonSize)) {
				ImGui::OpenPopup("Save As");
			}
			ImGui::SameLine();
			// Save As
			if (ImGui::Button("Save", buttonSize)) {
				ImGui::OpenPopup("Saving");
				mSaved = false;
			}

			ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
			ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
			if (ImGui::BeginPopupModal("Save As", nullptr, 0)) {
				static char inputBuffer[64] = "";
				ImGui::Text("Enter file name:");
				if (ImGui::InputText("", inputBuffer, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
					mSelectedMaterial = std::string(inputBuffer);
					memset(inputBuffer, 0, 64);
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
					ImGui::OpenPopup("Saving");
					mSaved = false;
				}
				else if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !ImGui::IsItemHovered()) {
					memset(inputBuffer, 0, 64);
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}
				else {
					ImGui::EndPopup();
				}
			}

			ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
			ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
			if (ImGui::BeginPopupModal("Saving", nullptr, 0)) {
				ImGui::Text("Saving... ");
				if (!mSaved) {
					SaveMaterial(pMaterial);
					mSaved = true;
				}
				ImGui::SameLine();
				ImGui::Text("Success!");
				if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}
		else {
			ImGui::Text("Failed to load Material...");
		}

		ImGui::End();
	}
}

void MaterialEditor::DrawDemoMaterial(Material* a_pMaterial)
{
	//a_pMaterial->mpShader->Use();
	mpDemoShader->Use();
	mpDemoFBO->Bind();

	glViewport(0, 0, mpDemoFBO->mWidth, mpDemoFBO->mHeight);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// Set Matrices
	mpDemoShader->SetUniform("modelMatrix", mDemoModel);
	mpDemoShader->SetUniform("viewMatrix", mDemoView);
	mpDemoShader->SetUniform("projectionMatrix", mDemoPerspective);

	// Lighting Values
	mpDemoShader->SetUniform("lightPos", 4.0f, 2.0f, 4.0f);
	mpDemoShader->SetUniform("eyePos", 0.0f, 0.0f, 2.0f);

	// Draw 
	Mesh* mesh = RESOURCE_MANAGER->LoadModel("Sphere")->mMeshes[0];

	// Set Material values
	Texture* pDiffuseTexture = a_pMaterial->mpDiffuseTexture;
	if (pDiffuseTexture) {
		pDiffuseTexture->Bind(mpDemoShader, "modelTexture", 0);
		mpDemoShader->SetUniform("UsesTexture", true);
	}
	else {
		mpDemoShader->SetUniform("UsesTexture", false);
		if (a_pMaterial->mDiffuseColor.w < 1.0f) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	Texture* pNormalMap = a_pMaterial->mpNormalMap;
	if (pNormalMap) {
		pNormalMap->Bind(mpDemoShader, "normalMap", 11);
		mpDemoShader->SetUniform("UsesNormalMap", true);
	}
	else {
		mpDemoShader->SetUniform("UsesNormalMap", false);
	}

	if (a_pMaterial->mAdditiveBlending) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	mpDemoShader->SetUniform("diffuse", a_pMaterial->mDiffuseColor.x, a_pMaterial->mDiffuseColor.y, a_pMaterial->mDiffuseColor.z, a_pMaterial->mDiffuseColor.w);
	mpDemoShader->SetUniform("specular", a_pMaterial->mSpecularColor.x, a_pMaterial->mSpecularColor.y, a_pMaterial->mSpecularColor.z);
	mpDemoShader->SetUniform("shininess", a_pMaterial->mShininess);

	mpDemoShader->SetUniform("UsesLighting", a_pMaterial->mUseLighting);

	// Draw Mesh
	glBindVertexArray(mesh->mVAO);
	glDrawElements(GL_TRIANGLES, (int)mesh->mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Unbind textures
	if (pDiffuseTexture)
		pDiffuseTexture->Unbind();

	if (pNormalMap)
		pNormalMap->Unbind();

	mpDemoFBO->Unbind();
	mpDemoShader->Unuse();
	//a_pMaterial->mpShader->Unuse();

	ImGui::Image((void*)(intptr_t)mpDemoFBO->GetTextureID(0), ImVec2(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvailWidth()), ImVec2(0, 1), ImVec2(1, 0));
}

void MaterialEditor::SaveMaterial(Material* a_pMaterial)
{
	if (a_pMaterial->mpDiffuseTexture)
		if (a_pMaterial->mRoot["DiffuseTexture"].isNull() || a_pMaterial->mRoot["DiffuseTexture"].asString() != a_pMaterial->mpDiffuseTexture->mName)
			a_pMaterial->mRoot["DiffuseTexture"] = a_pMaterial->mpDiffuseTexture->mName;
	else
		a_pMaterial->mRoot.removeMember("DiffuseTexture");

	if (a_pMaterial->mpNormalMap)
		if (a_pMaterial->mRoot["NormalMap"].isNull() || a_pMaterial->mRoot["NormalMap"].asString() != a_pMaterial->mpNormalMap->mName)
			a_pMaterial->mRoot["NormalMap"] = a_pMaterial->mpNormalMap->mName;
	else
		a_pMaterial->mRoot.removeMember("NormalMap");

	if (a_pMaterial->mRoot["Diffuse"].isNull() || 
		fabsf(a_pMaterial->mRoot["Diffuse"][0].asFloat() - a_pMaterial->mDiffuseColor.r) > 0.001f ||
		fabsf(a_pMaterial->mRoot["Diffuse"][1].asFloat() - a_pMaterial->mDiffuseColor.g) > 0.001f ||
		fabsf(a_pMaterial->mRoot["Diffuse"][2].asFloat() - a_pMaterial->mDiffuseColor.b) > 0.001f ||
		fabsf(a_pMaterial->mRoot["Diffuse"][3].asFloat() - a_pMaterial->mDiffuseColor.a) > 0.001f) {
		Json::Value diffuse(Json::arrayValue);
		diffuse.append(a_pMaterial->mDiffuseColor.r);
		diffuse.append(a_pMaterial->mDiffuseColor.g);
		diffuse.append(a_pMaterial->mDiffuseColor.b);
		diffuse.append(a_pMaterial->mDiffuseColor.a);
		a_pMaterial->mRoot["Diffuse"] = diffuse;
	}

	if (a_pMaterial->mRoot["Specular"].isNull() ||
		fabsf(a_pMaterial->mRoot["Specular"][0].asFloat() - a_pMaterial->mSpecularColor.r) > 0.001f ||
		fabsf(a_pMaterial->mRoot["Specular"][1].asFloat() - a_pMaterial->mSpecularColor.g) > 0.001f ||
		fabsf(a_pMaterial->mRoot["Specular"][2].asFloat() - a_pMaterial->mSpecularColor.b) > 0.001f) {
		Json::Value specular(Json::arrayValue);
		specular.append(a_pMaterial->mSpecularColor.r);
		specular.append(a_pMaterial->mSpecularColor.g);
		specular.append(a_pMaterial->mSpecularColor.b);
		a_pMaterial->mRoot["Specular"] = specular;
	}

	if (a_pMaterial->mRoot["Shininess"].isNull() || fabsf(a_pMaterial->mRoot["Shininess"].asFloat() - a_pMaterial->mShininess) > 0.001f)
		a_pMaterial->mRoot["Shininess"] = a_pMaterial->mShininess;

	if (a_pMaterial->mpShader)
		if (a_pMaterial->mRoot["Shader"].isNull() || a_pMaterial->mRoot["Shader"].asString() != a_pMaterial->mpShader->mName)
			a_pMaterial->mRoot["Shader"] = a_pMaterial->mpShader->mName;
	else
		a_pMaterial->mRoot.removeMember("Shader");

	if (a_pMaterial->mRoot["AdditiveBlending"].isNull() || a_pMaterial->mRoot["AdditiveBlending"].asBool() != a_pMaterial->mAdditiveBlending)
		a_pMaterial->mRoot["AdditiveBlending"] = a_pMaterial->mAdditiveBlending;

	if (a_pMaterial->mRoot["UseLighting"].isNull() || a_pMaterial->mRoot["UseLighting"].asBool() != a_pMaterial->mUseLighting)
		a_pMaterial->mRoot["UseLighting"] = a_pMaterial->mUseLighting;

	Json::StreamWriterBuilder builder;
	Json::StreamWriter* pWriter = builder.newStreamWriter();
	std::ofstream outputFile(".\\Resources\\Materials\\" + mSelectedMaterial + ".mat");
	pWriter->write(a_pMaterial->mRoot, &outputFile);
	outputFile.close();
}