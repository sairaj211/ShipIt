#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "LevelEditor.h"
#include "Engine.h"
#include "Managers/EntityManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/InputManager.h"	
#include "Managers/EventManager.h"	
#include "Managers/MemoryManager.h"	
#include "Managers/FrameRateManager.h"
#include "Entity.h"
#include "Components/Transform.h"
#include "Components/LocalLightComponent.h"
#include "Components/ModelComponent.h"
#include "Managers/CameraManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Managers/Renderer.h"
#include "Physics/PhysicsManager.h"
#include "Components/ParticlesComponent.h"
#include "Input/Mouse.h"
#include "Event.h"
#include "RayCast.h"
#include "Events/PauseEvent.h"
#include "MaterialEditor.h"
#include "ImGuizmo.h"
#include "Resources/JsonData.h"
#include "Utility.h"
#include "ImGui/imgui_internal.h"

LevelEditor* LevelEditor::mInstance = nullptr;
bool LevelEditor::mShow_PropertiesWindow = false;
bool LevelEditor::mShow_WarningWindow = false;
bool LevelEditor::mShow_LoadLevelWindow = false;
bool LevelEditor::mShow_SaveLevelWindow = false;
bool LevelEditor::mShow_ResetLevel = false;
int LevelEditor::timeStamp = 0;

LevelEditor::LevelEditor()
{
	runStep = false;
	isRun = false;
	isPause = false;
	isStep = false;

	mRun = false;
	isDebug = true;

	prevIsDebug = true;
	mMaterialEditorActive = false;
	currentEntity = nullptr;
	deleteAll = false;
	selected = -1;
	frame_num = 0;
	mManipulate = false;
	mBoundingBox = false;
	mShowCreateWindow = false;
	showContacts = false;
	imguiObjID = 1000;
	TRS = 0;
}

LevelEditor::~LevelEditor()
{
	delete mpMaterialEditor;
}

LevelEditor* LevelEditor::GetInstance()
{
	if (!mInstance) {
		mInstance = new LevelEditor();
	}
	return mInstance;
}

void LevelEditor::Init()
{
	LOG_INFO("Level Editor Init");
	mpMaterialEditor = new MaterialEditor();
	showTree = false;
}

void LevelEditor::PlayPauseStepBegin() {
	prevIsDebug = isDebug;
	prevRun = mRun;
	showTree = showTree;
	if (mRun) {

		if (isPause ) {
			
			isPause = false;
			isDebug = true;
			mRun = false;
			isRun = false;
			runStep = false;
		}
		if (!isStep && runStep) {
			mRun = false;
			runStep = false;
		}
	}
	else {
		if (isStep) {
			mRun = true;
			runStep = true;
		}
		if (isRun) {
			mRun = true;
		}
	}
	{
		bool temp = true;
		ImGui::Begin("Engine");
		const float DISTANCE = 10.0f;
		static int corner = 0;
		ImGuiIO& io = ImGui::GetIO();
		if (corner != -1)
		{
			ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
			ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
		if (ImGui::Begin("Example: Simple overlay", &temp, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
		{
			ImGui::Text(" %d ms/frame (%.2f FPS)", (Uint32)FRAMERATE_MANAGER->GetFrameTime(), 1000.0f / FRAMERATE_MANAGER->GetFrameTime());
			ImGui::Separator();
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
				if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
				if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
				if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
				if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
				ImGui::EndPopup();
			}
			ImGui::Text(" %lu tick number", frame_num);
		}
		ImGui::End();

		isRun = ImGui::Button("Run"); ImGui::SameLine();
		isPause = ImGui::Button("Pause"); ImGui::SameLine();
		isStep = ImGui::Button("Single Frame");
		ImGui::Checkbox("AABBTree", &showTree); ImGui::SameLine();
		ImGui::Checkbox("Contacts", &showContacts);
		ImGui::End();
	}
}

void LevelEditor::PlayPauseStepEnd() 
{
	if (prevRun != mRun) {
		PauseEvent* peEvent = static_cast<PauseEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("PauseEvent"));
		if (nullptr != peEvent) {
			peEvent->mPause = !mRun;
			//peEvent->mLayer = 0;
			EVENT_MANAGER->BroadcastEvent(peEvent);
		}
	}
	prevIsDebug = isDebug;
	prevRun = mRun;
}

void LevelEditor::Properties() {

	Transform* mtransform = GetTransform(selected);
	if (mtransform != nullptr) {
		//glm::mat4 identityMatrix(1.0f);
		ImGuizmo::BeginFrame();

		if (ImGui::Begin("Inspector", 0, ImGuiWindowFlags_NoCollapse))
		{
			glm::vec3 newScale, newRotate, newTrans;
			Entity* pEditEntity = ENTITY_MANAGER->GetEntity(selected, true);
			if (!pEditEntity)
				pEditEntity = ENTITY_MANAGER->GetRuntimeEntity(selected);

			if (pEditEntity) {
				//float height = ImGui::GetIO().DisplaySize.y;
				//float width = ImGui::GetIO().DisplaySize.x / 5.0f;
				//ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 325.0f, 100.0f));
				////float x = ImGui::GetWindowContentRegionMax().x;
				//ImGui::GetStyle().DisplaySafeAreaPadding.x = 0.0f;
				//ENABLED
				bool isEnabled = pEditEntity->IsEnabled();
				ImGui::Checkbox("", &isEnabled);
				if (isEnabled != pEditEntity->IsEnabled())
					ENTITY_MANAGER->EntityEnable(selected, isEnabled);
				ImGui::SameLine();
				//ID
				ImGui::Text("ID: %d", selected); ImGui::SameLine();
				int parentID = pEditEntity->GetParentID();
				//PARENT
				ImGui::Text(" Parent"); ImGui::SameLine();
				static char parentBuffer[64] = "";
				ImGui::PushItemWidth(35);
				if (ImGui::InputTextWithHint("##Parent", std::to_string(parentID).c_str(), parentBuffer, 10, ImGuiInputTextFlags_EnterReturnsTrue)) {
					pEditEntity->SetParentID(atoi(parentBuffer));
					memset(parentBuffer, 0, 10);
				}
				//TYPE
				ImGui::PopItemWidth(); ImGui::SameLine();
				ImGui::Text(std::string("Type: " + pEditEntity->GetType()).c_str());
				//NAME
				ImGui::Text("Name  ");
				ImGui::SameLine();
				static char inputBuffer[64] = "";
				if (ImGui::InputTextWithHint("##Name", pEditEntity->GetName().c_str(), inputBuffer, 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
					pEditEntity->SetName(std::string(inputBuffer));
					memset(inputBuffer, 0, 64);
				}
				ImGui::Separator();
				//TRANSFORM
				ImGuizmo::EditTransform(&CAMERA_MANAGER->GetCameraViewMatrix(), &CAMERA_MANAGER->GetCameraProjectionMatrix(),
					&mtransform->ComputeModelMatrix(), &newScale, &newRotate, &newTrans, &mManipulate, &TRS, mBoundingBox);

				mtransform->SetPosition(newTrans);
				mtransform->SetRotationAngles(newRotate);
				mtransform->SetScale(newScale);
				//COMPONENTS
				std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(selected, true);
				for (Component* pComeponent : components) {
					ImGui::Separator();
					if (pComeponent->GetType() != "Transform" && ImGui::TreeNode(pComeponent->GetType().c_str())) {
						pComeponent->DisplayDebug();
						ImGui::TreePop();
					}
				}
				/*if (ImGui::Button("Delete Object")){DeleteObject();	}*/
			}
			ImGui::End();
		}
	}
}

void LevelEditor::ImGUICreateFiltered(std::string strn) 
{
	std::vector<std::string> models = GetUniqueDirectoryFiles(".\\Resources\\Prefab\\");
	std::map<int, std::string> prefabList;
	for (std::string prefabName : models) {
		if (prefabName.find(strn) != std::string::npos)
			prefabList.insert(std::pair<int, std::string>(imguiObjID++, prefabName));
	}
	for (std::pair<int, std::string> element : prefabList) {
		if (ImGui::MenuItem(element.second.c_str())) {
			if (element.second == "Teleport")
			{
				for (int i = 1; i < 5; ++i) {
					Entity* temp = ENTITY_MANAGER->CreateEntity(element.second);

					std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(temp->GetID());

					for (Component* compo : components)
					{
						compo->Init();
						if (compo->GetType() == "Teleport") {
							Teleport* mTeleport = static_cast<Teleport*>(COMPONENT_MANAGER->GetComponent<Teleport>(temp->GetID(), "Teleport", true));
							mTeleport->mPairID = mLastPairID + 1;
							if (i % 2 == 0)
								mLastPairID++;
						}

					}

					std::string currentLevel = SCENE_MANAGER->GetCurrent();
					SCENE_MANAGER->AddEntity(temp->GetID(), currentLevel);

					selected = temp->GetID(); mShow_PropertiesWindow = true;
					AddToRecentList(selected);
					timeStamp++;
				}
			}
			else
			{
				Entity* temp = ENTITY_MANAGER->CreateEntity(element.second);
				std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(temp->GetID());
				for (Component* compo : components) {
					compo->Init();
				}
				std::string currentLevel = SCENE_MANAGER->GetCurrent();
				SCENE_MANAGER->AddEntity(temp->GetID(), currentLevel);
				selected = temp->GetID(); mShow_PropertiesWindow = true;
				AddToRecentList(selected);
				timeStamp++;
			}
		}
	}
}

void LevelEditor::ImGUICreateExact(std::string strn)
{
	std::vector<std::string> models = GetUniqueDirectoryFiles(".\\Resources\\Prefab\\");
	std::map<int, std::string> prefabList;
	for (std::string prefabName : models) {
		if (prefabName==strn)
			prefabList.insert(std::pair<int, std::string>(imguiObjID++, prefabName));
	}
	for (std::pair<int, std::string> element : prefabList) {
	if (ImGui::MenuItem(element.second.c_str())) {
			Entity* temp = ENTITY_MANAGER->CreateEntity(element.second);
			std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(temp->GetID());
			for (Component* compo : components) {
				compo->Init();
			}
			std::string currentLevel = SCENE_MANAGER->GetCurrent();
			SCENE_MANAGER->AddEntity(temp->GetID(), currentLevel);
			selected = temp->GetID(); mShow_PropertiesWindow = true;
			AddToRecentList(selected);
			timeStamp++;
		}
	}

}
void LevelEditor::MenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::BeginMenu("Load")) {
					std::vector<std::string> levelName = GetUniqueDirectoryFiles(".\\Resources\\Level\\");
					for (std::string fileName : levelName) {
						if (ImGui::MenuItem(fileName.c_str() )) {
							deleteAll = true;
							timeStamp = 0;
							HideWindows();
							SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
							SCENE_MANAGER->PushScene(fileName);
							OBJECT_FACTORY->LoadLevel(fileName);
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::Button("Save ")) { ImGui::OpenPopup("Save"); }
				if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					bool dummyWindow = true;

					ImGui::Separator();
					static char inputBuffer[64] = "SaveLevel";
					ImGui::Text("Enter file name:");
					ImGui::InputText("default", inputBuffer, 64);
					std::string mSaveFile = std::string(inputBuffer);

					ImVec2 buttonsize(50, 20);
					if (ImGui::Button("Save", buttonsize))
					{
						if (mSaveFile == "")
							//warningWindow = true;
							ImGui::OpenPopup("Warning Window");
						else {
							mShow_SaveLevelWindow = false;
							OBJECT_FACTORY->SaveLevel(mSaveFile);
						}
					}
					ImGui::SameLine();

					ImGui::Dummy(ImVec2(80.0f, 20.0f));
					ImGui::SameLine();

					if (ImGui::BeginPopupModal("Warning Window", &dummyWindow, ImGuiWindowFlags_AlwaysAutoResize)) {
						ImGui::Text("SAVE file name cannot be null string");
						ImGui::Text("Enter a Valid file name");

						if (ImGui::Button("Close")) {
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				if (ImGui::Button("New  ")) {
					for (std::pair<int, Entity*> element : ENTITY_MANAGER->mEntityMap) {
						if (element.second != nullptr) {
							if (element.second->GetDataFile() == "Skybox" || element.second->GetDataFile() == "DRLight")
								LOG_INFO("Dont");
							else
								ENTITY_MANAGER->DeleteEntity(element.first);
						}
					}
					Entity* temp = ENTITY_MANAGER->CreateRuntimeEntity("Cube");
					std::vector<Component*> components = COMPONENT_MANAGER->GetRuntimeComponents(temp->GetID());
					for (Component* compo : components) {
						compo->Init();
					}
					Transform* mtransform = static_cast<Transform*>(COMPONENT_MANAGER->GetRuntimeComponent<Transform>(temp->GetID(), "Transform"));

					glm::vec3 scale = mtransform->GetScale();
					scale.x = 10.0f;
					scale.y = 0.2f;
					scale.z = 10.0f;
					mtransform->SetScale(scale);
					HideWindows();
				}
				if (ImGui::Button("Reset"))
				{
					std::string fileName = SCENE_MANAGER->GetCurrent();
					SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
					SCENE_MANAGER->PushScene(fileName);
					OBJECT_FACTORY->LoadLevel(fileName);
					deleteAll = true;
					timeStamp = 0;
					HideWindows();
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Prefab", "Cntrl+Alt+S")) {
				if (ImGui::Button("Save "))
					ImGui::OpenPopup("Save Prefab");
				std::string prefabName = "";
				ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
				ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
				if (ImGui::BeginPopupModal("Save Prefab", nullptr, 0)) {
					Entity* pEntity = ENTITY_MANAGER->GetEntity(selected, true);
					if (pEntity) {
						static char inputBuffer[64] = "";
						ImGui::Text("Enter prefab name:");
						if (ImGui::InputText("", inputBuffer, 64, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank)) {
							prefabName = std::string(inputBuffer);
							memset(inputBuffer, 0, 64);
							ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
							ImGui::OpenPopup("PrefabSaving");
							savedPrefab = false;
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
					else {
						ImGui::Text("Select an object before");
						ImGui::Text("saving.");
						if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !ImGui::IsItemHovered())
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}
				}

				ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
				ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
				if (ImGui::BeginPopupModal("PrefabSaving", nullptr, 0)) {
					ImGui::Text("Saving... ");
					if (!savedPrefab) {
						Json::StreamWriterBuilder builder;
						Json::StreamWriter* pWriter = builder.newStreamWriter();
						std::ofstream outputFile(".\\Resources\\Prefab\\" + prefabName + ".json");
						Json::Value root;
						Entity* pEntity = ENTITY_MANAGER->GetEntity(selected, true);
						pEntity->DeSerialize(root, true);
						pWriter->write(root, &outputFile);
						outputFile.close();
						savedPrefab = true;
					}

					ImGui::SameLine();
					ImGui::Text("Success!");
					if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
						ImGui::CloseCurrentPopup();

					if (ImGui::Button("Close")) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}//Prefab Saving
				if (ImGui::BeginMenu("Load")) {

					ImGUICreateFiltered("Prf_");

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			//if (ImGui::Button("Test  ")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject")) {
			if (ImGui::BeginMenu("Light")) {ImGUICreateFiltered("Light");	ImGui::EndMenu();}
			if (ImGui::BeginMenu("Particle")) { ImGUICreateFiltered("Pt");	ImGui::EndMenu(); }
			if (ImGui::BeginMenu("Background")) { ImGUICreateFiltered("Bg");	ImGui::EndMenu(); }
			ImGUICreateExact("Cube");
			ImGUICreateExact("Group");
			ImGUICreateExact("Camera");
			if (ImGui::BeginMenu("All")) { ImGUICreateFiltered("");	ImGui::EndMenu(); }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			 
			if (ImGui::MenuItem("Materials")) { mMaterialEditorActive = true; }
			ImGui::EndMenu();
		}
	
		ImGui::EndMainMenuBar();
	}
}

void LevelEditor::Raycast() {
	glm::vec2  mousePos = Mouse::GetInstance()->GetMouseScreenPos();
	if (Mouse::GetInstance()->IsTriggered(LEFT) && !ImGui::IsAnyWindowHovered() && !ImGui::GetIO().WantCaptureMouse && !mManipulate && !ImGuizmo::isOverAnchor()) {
		selected = RAYCAST->CastRay(mousePos.x, mousePos.y, false);
	}
}

void LevelEditor::Hierarchy() {
	ImGui::SetNextWindowPos(ImVec2(0.0f, 20.0));
	ImGui::GetStyle().DisplaySafeAreaPadding.x = 0.0f;
	if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_NoCollapse)) {
		char * actualScene = const_cast<char*>(OBJECT_FACTORY->mCurrentLevel.c_str());
		//ImGui::Checkbox("Material Editor", &LEVEL_EDITOR->mMaterialEditorActive);
		if (ImGui::TreeNode(actualScene)) {
			std::unordered_map<int, Entity*> entities = ENTITY_MANAGER->mEntityMap;
			for (std::pair<int, Entity*> element : entities) {
				Entity* pEntity = element.second;
				if (pEntity && pEntity->GetParentID() == -1) {
					DrawEntityNode(pEntity);
				}
			}
			ImGui::TreePop();
		}
		MenuBar();
		ImGui::End();
	}
}

void LevelEditor::HighLightSelected() {
	ModelComponent* pSelectedModel = COMPONENT_MANAGER->GetComponent<ModelComponent>(selected, "ModelComponent");
	if (pSelectedModel) {
		Outline outline;
		outline.pModelComponent = pSelectedModel;
		outline.respectOcclusion = false;
		outline.color = glm::vec3(1.0f, 1.0, 0.0f);
		RENDERER->PushToOutlineQueue(outline);
	}
}

void LevelEditor::DrawEntityNode(Entity* a_pEntity)
{
	int id = a_pEntity->GetID();
	char name[256];
	std::vector<int> children = a_pEntity->GetChildIDs();
	std::vector<Entity*> realChildren;
	for (int i = 0; i < children.size(); ++i) {
		Entity* pChild = ENTITY_MANAGER->GetEntity(children[i], true);
		if (pChild) {
			realChildren.push_back(pChild);
		}
	}
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (selected == id) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (realChildren.size() == 0) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	snprintf(name, 256, "%-4d %s", id, a_pEntity->GetName().c_str());
	bool open = ImGui::TreeNodeEx((void*)(intptr_t)id, flags, name);
	if (ImGui::IsItemClicked()) {
		selected = id;
		mShow_PropertiesWindow = true;
	}
	if (open) {
		for (int i = 0; i < realChildren.size(); ++i) {
			DrawEntityNode(realChildren[i]);
		}

		if (realChildren.size() > 0) {
			ImGui::TreePop();
		}
	}
}

GEAR_API bool LevelEditor::Run()
{
	return mRun;
}

void LevelEditor::CleanUp()
{
	delete mInstance;
}

GEAR_API void LevelEditor::AddToRecentList(int entity_ID)
{
	recentlyCreatedObjects.push_back(std::pair(entity_ID, timeStamp));
}

GEAR_API void LevelEditor::DeleteList()
{
	for (unsigned int i = 0; i < deleteThem.size(); ++i)  {
		redoList.push_back(deleteThem[i].second);
		ENTITY_MANAGER->DeleteEntity(deleteThem[i].first);
	}
	timeStamp--;
	deleteThem.clear();
}

GEAR_API void LevelEditor::Undo()
{
	if (!recentlyCreatedObjects.empty()) {
		for (size_t i = recentlyCreatedObjects.size() - 1; i >= 0; --i) 	{
			if (recentlyCreatedObjects[i].second == timeStamp - 1) {
				Entity* element = ENTITY_MANAGER->GetEntity(recentlyCreatedObjects[i].first);
				deleteThem.push_back(std::pair(recentlyCreatedObjects[i].first, element));
				recentlyCreatedObjects.erase(recentlyCreatedObjects.begin() + (i));
			}
			if (i == 0)break;
		}

	}
	if (!deleteThem.empty())
		DeleteList();
}

GEAR_API void LevelEditor::Duplicate(int selectedObj)
{
	Entity* temp = ENTITY_MANAGER->GetEntity(selectedObj);
	Json::Value clipboard(Json::arrayValue);
	Json::Value dupEntity;
	dupEntity["DataFile"] = temp->GetDataFile();
	temp->DeSerialize(dupEntity, true);
	clipboard.append(dupEntity);
	std::vector<int> newIDs = OBJECT_FACTORY->LoadEntities(clipboard, false, temp->GetParentID());
	selectedObj = newIDs[0];
	std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(selectedObj);
	for (Component* compo : components) {
		compo->Init();
	}
	mShow_PropertiesWindow = true;
	AddToRecentList(selectedObj);
	selected = selectedObj;
	timeStamp++;
}

GEAR_API Transform* LevelEditor::GetTransform(int a_ID)
{
	Entity* temp = ENTITY_MANAGER->GetEntity(a_ID, true);
	if (temp != nullptr) {
		Transform* mtransform = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(temp->GetID(), "Transform", true));
		return mtransform;
	}
	else
		return nullptr;
}

void LevelEditor::ChangeLevel()
{
	for (std::pair<int, Entity*> element : ENTITY_MANAGER->mEntityMap) {
		ENTITY_MANAGER->DeleteEntity(element.first);
	}
	deleteAll = true;
}

GEAR_API void LevelEditor::ShortcutKeys()
{
	if (INPUT_MANAGER->IsReleased(SDL_SCANCODE_W) && !INPUT_MANAGER->IsPressed("click_right", -1))	TRS = 0;	//Translate
	if (INPUT_MANAGER->IsReleased(SDL_SCANCODE_E) && !INPUT_MANAGER->IsPressed("click_right", -1))	TRS = 1;	//Rotate
	if (INPUT_MANAGER->IsReleased(SDL_SCANCODE_R) && !INPUT_MANAGER->IsPressed("click_right", -1))	TRS = 2;	//Scale
	if (INPUT_MANAGER->IsReleased(SDL_SCANCODE_T) && !INPUT_MANAGER->IsPressed("click_right", -1)){ if (TRS == 2) mBoundingBox = !mBoundingBox; }
	switch (TRS) { case 0: case 1: mBoundingBox = false; }
	bool zFlag = false;
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_S)) {
		mShow_SaveLevelWindow = !mShow_SaveLevelWindow;
		LOG_INFO("Level Saved");
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LALT) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_S)) {
		ImGui::OpenPopup("Save Prefab");
	}
	if (INPUT_MANAGER->IsTriggered(SDL_SCANCODE_ESCAPE)) {
		HideWindows();
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_D)) {
		if (selected != -1) {
			Duplicate(selected);
			LOG_INFO("Duplicate");
		}
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_R))		{
		selected = -1;
		for (std::pair<int, Entity*> element : ENTITY_MANAGER->mEntityMap) {
			ENTITY_MANAGER->DeleteEntity(element.first);
		}
		deleteAll = true;
		LOG_INFO("Reset Level");
		HideWindows();
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_O)) 	{
		mShow_LoadLevelWindow = !mShow_LoadLevelWindow;
		LOG_INFO("Load");
	}

	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_P)) 	{
		LOG_INFO("Play/Pause");

		//mRun = !mRun;
		//	if (isPause) {//Go Pause
		//		isPause = false;
		//		isDebug = true;//raycast
		//		mRun = false;
		//		isRun = false;
		//	}
		//	else {//GO PLAY
		//		isPause = true;
		//		isDebug = false;
		//		mRun = true;
		//		isRun = true;
		//	}
	}
		if (false){	//if ( INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_Z)){
		Undo();
		LOG_INFO("Undo");
	}
	if (INPUT_MANAGER->IsTriggered(SDL_SCANCODE_DELETE)){
		if (selected != 1)
			DeleteObject();
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LALT)){
		zFlag = true;
	}
	//SCALING
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_RIGHT))	{
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.x += 0.2f;
		mtransform->SetScale(tempScale);
	}

	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_LEFT)){
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.x -= 0.2f;
		mtransform->SetScale(tempScale);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_UP) && !zFlag){
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.y += 0.2f;
		mtransform->SetScale(tempScale);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_DOWN) && !zFlag){
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.y -= 0.2f;
		mtransform->SetScale(tempScale);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_UP) && zFlag) {
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.z -= 0.2f;
		mtransform->SetScale(tempScale);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_DOWN) && zFlag) 	{
		Transform* mtransform = GetTransform(selected);
		glm::vec3 tempScale = mtransform->GetScale();
		tempScale.z += 0.2f;
		mtransform->SetScale(tempScale);
	}
	//TRANSFORM
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_RIGHT)) 	{
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.x += 0.2f;
		mtransform->SetPosition(temptrans);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_LEFT)) {
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.x -= 0.2f;
		mtransform->SetPosition(temptrans);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_UP) && !zFlag) {
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.y += 0.2f;
		mtransform->SetPosition(temptrans);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_DOWN) && !zFlag) 	{
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.y -= 0.2f;
		mtransform->SetPosition(temptrans);;
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_UP) && zFlag) {
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.z -= 0.2f;
		mtransform->SetPosition(temptrans);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsPressed(SDL_SCANCODE_DOWN) && zFlag) {
		Transform* mtransform = GetTransform(selected);
		glm::vec3 temptrans = mtransform->GetPosition();
		temptrans.z += 0.2f;
		mtransform->SetPosition(temptrans);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LSHIFT) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_SPACE)) {
		HideWindows();
	}
}

void LevelEditor::SaveScenePopup() 
{
	if (mShow_SaveLevelWindow) {
		ImGui::OpenPopup("Save");
		if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			static char inputBuffer[64] = "SaveLevel";
			ImGui::Text("Enter file name:");
			ImGui::InputText("default", inputBuffer, 64);
			std::string mSaveFile = std::string(inputBuffer);

			ImVec2 buttonsize(50, 20);
			if (ImGui::Button("Save", buttonsize)) {
				if (mSaveFile == "")
					ImGui::OpenPopup("Warning Window");
				else {
					mShow_SaveLevelWindow = false;
					OBJECT_FACTORY->SaveLevel(mSaveFile);
				}
			}
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(80.0f, 20.0f));
			ImGui::SameLine();
			if (ImGui::Button("Close", buttonsize)) {
				mShow_SaveLevelWindow = false;
				ImGui::CloseCurrentPopup();
			}

			bool dummyWindow = true;
			if (ImGui::BeginPopupModal("Warning Window", &dummyWindow, ImGuiWindowFlags_AlwaysAutoResize)) {

				ImGui::Text("SAVE file name cannot be null string");
				ImGui::Text("Enter a Valid file name");
				if (ImGui::Button("Close")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::EndPopup();
		}
	}
}

void LevelEditor::OpenScenePopup() 
{
	if (mShow_LoadLevelWindow) {
		ImGui::OpenPopup("Load");
		float height = 200.0f;
		float width = 300.0f;
		ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 300.0f, ImGui::GetIO().DisplaySize.y / 2));
		float x = ImGui::GetWindowContentRegionMax().x;
		ImGui::SetNextWindowSize(ImVec2(x, height));
		ImGui::GetStyle().DisplaySafeAreaPadding.x = 0.0f;
		if (ImGui::BeginPopupModal("Load", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::BeginCombo("Load##Load", OBJECT_FACTORY->mCurrentLevel.c_str())) {
				std::vector<std::string> levelName = GetUniqueDirectoryFiles(".\\Resources\\Level\\");
				for (std::string fileName : levelName) {
					bool isSelected = (OBJECT_FACTORY->mCurrentLevel == fileName);
					if (ImGui::Selectable(fileName.c_str(), isSelected)) {
						deleteAll = true;
						timeStamp = 0;
						HideWindows();
						SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
						SCENE_MANAGER->PushScene(fileName);
						OBJECT_FACTORY->LoadLevel(fileName);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Dummy(ImVec2(80.0f, 20.0f));
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(80.0f, 20.0f));
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(80.0f, 20.0f));
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				mShow_LoadLevelWindow = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}

void LevelEditor::SavePrefabPopup()
{
	std::string prefabName = "";
	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
	ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
	if (ImGui::BeginPopupModal("Save Prefab", nullptr, 0)) {
		Entity* pEntity = ENTITY_MANAGER->GetEntity(selected, true);
		if (pEntity) {
			static char inputBuffer[64] = "";
			ImGui::Text("Enter prefab name:");
			if (ImGui::InputText("", inputBuffer, 64, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank)) {
				prefabName = std::string(inputBuffer);
				memset(inputBuffer, 0, 64);
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				ImGui::OpenPopup("PrefabSaving");
				savedPrefab = false;
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
		else {
			ImGui::Text("Select an object before");
			ImGui::Text("saving.");
			if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !ImGui::IsItemHovered())
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 100.0f, ImGui::GetIO().DisplaySize.y / 2));
	ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
	if (ImGui::BeginPopupModal("PrefabSaving", nullptr, 0)) {
		ImGui::Text("Saving... ");
		if (!savedPrefab) {
			Json::StreamWriterBuilder builder;
			Json::StreamWriter* pWriter = builder.newStreamWriter();
			std::ofstream outputFile(".\\Resources\\Prefab\\" + prefabName + ".json");
			Json::Value root;
			Entity* pEntity = ENTITY_MANAGER->GetEntity(selected, true);
			pEntity->DeSerialize(root, true);
			pWriter->write(root, &outputFile);
			outputFile.close();
			savedPrefab = true;
		}

		ImGui::SameLine();
		ImGui::Text("Success!");
		if (Mouse::GetInstance()->IsTriggered(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

GEAR_API void LevelEditor::DeleteObject()
{
	Entity* temp = ENTITY_MANAGER->GetEntity(selected);
	ENTITY_MANAGER->DeleteEntity(selected);
	HideWindows();
}

GEAR_API void LevelEditor::HideWindows()
{
	mMaterialEditorActive = false;
	mShow_PropertiesWindow = false;
	mShow_WarningWindow = false;
	mShow_LoadLevelWindow = false;
	mManipulate = false;
	mShowCreateWindow = false;
	mShow_SaveLevelWindow = false;
	selected = -1;
}
GEAR_API void LevelEditor::OpenWindows()
{
	if (mMaterialEditorActive) {
		mpMaterialEditor->Draw(mMaterialEditorActive);
	}
}

void LevelEditor::Update()
{
	PlayPauseStepBegin();

	if (isDebug && !mRun) {
		Raycast(); //ask if pointed an element
		if (selected != -1) {
			HighLightSelected();
			Properties();
		}
		Hierarchy();
		OpenWindows();// for the moment just Material editor
		OpenScenePopup();
		SaveScenePopup();
		SavePrefabPopup();
		ShortcutKeys();
	}
	else {
		frame_num += 1;
		HideWindows();
	}
	PlayPauseStepEnd();
	imguiObjID = 1000;
}