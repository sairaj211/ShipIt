#if 0
void EditTransform(const Camera& camera, matrix_t& matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix.m16, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m16);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	vec_t snap;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(camera.mView.m16, camera.mProjection.m16, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m16, NULL, useSnap ? &snap.x : NULL);
}
#endif
#pragma once

namespace ImGuizmo
{
	// call inside your own window and before Manipulate() in order to draw gizmo to that window.
	GEAR_API void SetDrawlist();

	// call BeginFrame right after ImGui_XXXX_NewFrame();
	GEAR_API void BeginFrame();

	// return true if mouse cursor is over any gizmo control (axis, plan or screen component)
	GEAR_API bool IsOver();

	GEAR_API bool isOverAnchor();

	// return true if mouse IsOver or if the gizmo is in moving state
	GEAR_API bool IsUsing();

	// enable/disable the gizmo. Stay in the state until next call to Enable.
	// gizmo is rendered with gray half transparent color when disabled
	GEAR_API void Enable(bool enable);

	// helper functions for manualy editing translation/rotation/scale with an input float
	// translation, rotation and scale float points to 3 floats each
	// Angles are in degrees (more suitable for human editing)
	// example:
	// float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	// ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
	// ImGui::InputFloat3("Tr", matrixTranslation, 3);
	// ImGui::InputFloat3("Rt", matrixRotation, 3);
	// ImGui::InputFloat3("Sc", matrixScale, 3);
	// ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);
	//
	// These functions have some numerical stability issues for now. Use with caution.
	GEAR_API void DecomposeMatrixToComponents(const glm::mat4 *matrix, float *translation, float *rotation, float *scale);
	GEAR_API void RecomposeMatrixFromComponents(const float *translation, const float *rotation, const float *scale, glm::mat4 *matrix);

	GEAR_API void SetRect(float x, float y, float width, float height);
	// default is false
	GEAR_API void SetOrthographic(bool isOrthographic);

	// Render a cube with face color corresponding to face normal. Usefull for debug/tests
	GEAR_API void DrawCube(const glm::mat4 *view, const glm::mat4 *projection, const glm::mat4 *matrix);
	GEAR_API void DrawGrid(const glm::mat4 *view, const glm::mat4 *projection, const glm::mat4 *matrix, const float gridSize);

	// call it when you want a gizmo
	// Needs view and projection matrices. 
	// matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional
	// translation is applied in world space
	enum OPERATION
	{
		TRANSLATE,
		ROTATE,
		SCALE,
		BOUNDS,
	};

	enum MODE
	{
		LOCAL,
		WORLD
	};

	GEAR_API void Manipulate(const glm::mat4 *view, const glm::mat4 *projection, OPERATION operation, MODE mode, glm::mat4 *matrix, float *deltaMatrix = 0, float *snap = 0, float *localBounds = NULL, float *boundsSnap = NULL);
   //
   // Please note that this cubeview is patented by Autodesk : https://patents.google.com/patent/US7782319B2/en
   // It seems to be a defensive patent in the US. I don't think it will bring troubles using it as
   // other software are using the same mechanics. But just in case, you are now warned!
   //
	GEAR_API void ViewManipulate(glm::mat4* view, float length, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
	GEAR_API void EditTransform(const glm::mat4 *cameraView, const glm::mat4 *cameraProjection, glm::mat4* matrix, 
		glm::vec3* newScale, glm::vec3* newRotate, glm::vec3* newTrans, bool*, int *TRS, bool);

};
