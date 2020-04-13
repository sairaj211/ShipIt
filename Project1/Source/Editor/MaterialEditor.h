#pragma once
class FBO;
class Material;
class Shader;

/*!
 * Manages the creation, deletion,
 * and modification of Entity Materials
 * through a GUI.
 *
 */
class MaterialEditor
{
public:
	/*!
	 * Material Default Constructor
	 *
	 */
	GEAR_API MaterialEditor();
	/*!
	 * Material Default Destructor
	 *
	 */
	GEAR_API ~MaterialEditor();
	/*!
	 * Draws the windows used to interface with
	 * the Material Editor
	 *
	 * \param a_active A flag indicating whether or not the Material Editor is active
	 */
	GEAR_API void Draw(bool& a_active);

private:
	GEAR_API void DrawMaterialList(bool& a_active);
	GEAR_API void DrawMaterialEditor();
	GEAR_API void DrawDemoMaterial(Material* a_pMaterial);
	GEAR_API void SaveMaterial(Material* a_pMaterial);

private:
	std::string mSelectedMaterial;
	bool mEditActive;
	bool mAddPopup;
	bool mDragging;
	bool mSaved;
	FBO* mpDemoFBO;
	Shader* mpDemoShader;
	glm::mat4 mDemoModel;
	glm::mat4 mDemoView;
	glm::mat4 mDemoPerspective;
};