#pragma once
#include "../Component.h"

#define DRAW_WIREFRAME 1
#define DRAW_AABB 2
#define DRAW_VELOCITY 4
#define DRAW_COLLIDER 8

class Model;
class Material;

/*!
 * Manages the Model of the Entity
 * and its Materials
 * 
 */
class ModelComponent : public Component
{
public:
	/*!
	 * ModelComponent Default Constructor
	 * 
	 */
	GEAR_API ModelComponent();
	/*!
	 * ModelComponent Default Destructor
	 * 
	 */
	GEAR_API virtual ~ModelComponent();
	/*!
	 * Resets the ModelComponent to its default
	 * state
	 * 
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new ModelComponent
	 * 
	 * \return A pointer to a new ModelComponent
	 */
	GEAR_API ModelComponent* Create();
	/*!
	 * Push the ModelComponent to the appropriate
	 * render queue
	 * 
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the ModelComponent so
	 * that they are modifiable through ImGui
	 * 
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the ModelComponent with the
	 * data stored in the given Json data
	 * 
	 * \param a_root The root of the Json file that contains the ModelComponent data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the ModelComponent data into the given
	 * Json root
	 * 
	 * \param root The root of the Json file that the ModelComponent data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Randomly sets the rgba values of the
	 * Materials associated with each of the
	 * Model's Meshes
	 *
	 */
	GEAR_API void MakeRainbow();
	/*!
	 * Highlight the model with the its diffuse
	 * color
	 *
	 */
	GEAR_API void Highlight();
	/*!
	 * Toggles the debug drawing for collider
	 * information
	 *
	 */
	GEAR_API void ToggleDrawCollider();
	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);

private:
	GEAR_API int GetTransparency();

public:
	Material* mpMaterial; /*!< Material of the entire Model */
	Model* mpModel; /*!< Model of the Entity */
	std::vector<Material*> mMeshMaterials; /*!< Vector of Materials associated with each of the Model's Meshes */
	bool mIsRenderable; /*!< Flag indicating whether the Model is renderable or not */
	bool mCastsShadow; /*!< Flag indicating whether the Model casts a shadow or not */
	int mDebugDraw; /*!< Debug Variable that indicates what type of debug drawing should be done */
	bool mIsUI; /*!< Debug Variable that indicates what type of debug drawing should be done */
	bool mIsBillboard; /*!< Flag indicating whether the model should be rendered as a billboard or not */

	bool drawCollider;
private:
	int mEditMeshMaterial;
};