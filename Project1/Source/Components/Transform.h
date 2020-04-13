#pragma once

#include "Component.h"

#define POSITION_BIT 1
#define ROTATION_BIT 2
#define SCALE_BIT 4

/*!
 * Manages the Entity's position,
 * rotation, and scale
 * 
 */
class Transform : public Component {
public:
	/*!
	 * Transform Default Constructor
	 * 
	 */
	GEAR_API Transform();
	/*!
	 * Transform Default Destructor
	 * 
	 */
	GEAR_API virtual ~Transform();
	/*!
	 * Resets the Transform to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new Transform
	 * 
	 * \return A pointer to a new Transform
	 */
	GEAR_API Transform* Create();
	/*!
	 * Update the Transform values according
	 * to the given frame time
	 * 
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Transform so that they 
	 * are modifiable through ImGui
	 * 
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initializes the Transform with the data
	 * stored in the given Json data
	 * 
	 * \param a_root The root of the Json file that contains the Transform data
	*/
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the Transform data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Transform data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Adjust the Transform values based
	 * the Entity's new parent
	 *
	 */
	GEAR_API void OnSetParent();
	/*!
	 * Undo the adjustment on the Transform
	 * values
	 *
	 */
	GEAR_API void OnRemoveParent();
	/*!
	 * Sets the Transform's dirty flag to true
	 *
	 */
	GEAR_API void SetDirty(int a_dirtyBits = POSITION_BIT | ROTATION_BIT | SCALE_BIT);
	/*!
	 * Compute the model matrix for rendering
	 * 
	 * \return The 4x4 model matrix
	 */
	GEAR_API glm::mat4 ComputeModelMatrix();
	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 * Get the x, y, & z position of the transform
	 *
	 * \return A tuple of the x, y, and z position values
	 */
	GEAR_API std::vector<float> GetTuplePosition();
	/*!
	 * Sets the position of the Entity to the
	 * given xyz values
	 *
	 * \param a_x The new value of the x position
	 * \param a_y The new value of the y position
	 * \param a_z The new value of the z position
	 */
	GEAR_API void SetPositionValues(float a_x, float a_y, float a_z);
	/*!
	 * Sets the position of the Entity to the
	 * given xyz values relative to its parent
	 *
	 * \param a_x The new value of the x position
	 * \param a_y The new value of the y position
	 * \param a_z The new value of the z position
	 */
	GEAR_API void SetLocalPositionValues(float a_x, float a_y, float a_z);
	/*!
	 * Get the x, y, & z scale of the transform
	 *
	 * \return A tuple of the x, y, and z scale values
	 */
	GEAR_API std::vector<float> GetTupleScale();
	/*!
	 * Sets the scale of the Entity to the
	 * given xyz values
	 *
	 * \param a_x The new value of the x scale
	 * \param a_y The new value of the y scale
	 * \param a_z The new value of the z scale
	 */
	GEAR_API void SetScaleValues(float a_x, float a_y, float a_z);
	/*!
	 * Sets the scale of the Entity to the
	 * given xyz values relative to its parent
	 *
	 * \param a_x The new value of the x scale
	 * \param a_y The new value of the y scale
	 * \param a_z The new value of the z scale
	 */
	GEAR_API void SetLocalScaleValues(float a_x, float a_y, float a_z);
	/*!
	 * Sets the position of the Entity
	 *
	 * \param a_position The new position of the Entity
	 */
	GEAR_API void SetPosition(glm::vec3 a_position);
	/*!
	 * Gets the position of the Entity
	 *
	 * \return The position of the Entity
	 */
	GEAR_API glm::vec3 GetPosition();
	/*!
	 * Sets the local position of the Entity
	 * relative to its parent
	 *
	 * \param a_position The new local position of the Entity
	 */
	GEAR_API void SetLocalPosition(glm::vec3 a_position);
	/*!
	 * Gets the position of the Entity relative to
	 * its parent
	 *
	 * \return The position of the Entity relative to its parent
	 */
	GEAR_API glm::vec3 GetLocalPosition();
	/*!
	 * Sets the rotation angles of the Entity to the
	 * given xyz values
	 *
	 * \param a_x The new value of the x rotation
	 * \param a_y The new value of the y rotation
	 * \param a_z The new value of the z rotation
	 */
	GEAR_API void SetRotationAngleValues(float a_x, float a_y, float a_z);
	/*!
	 * Get the x, y, & z rotation angles of the transform
	 *
	 * \return A tuple of the x, y, and z rotation angle values
	 */
	GEAR_API std::vector<float> GetRotationAngleValues();
	/*!
	 * Sets the rotation angles of the Entity
	 *
	 * \param a_rotationAngles The new rotation angles of the Entity
	 */
	GEAR_API void SetRotationAngles(glm::vec3 a_rotationAngles);
	/*!
	 * Gets the rotation angles of the Entity 
	 *
	 * \return The rotation angles of the Entity
	 */
	GEAR_API glm::vec3 GetRotationAngles();
	/*!
	 * Sets the rotation angles of the Entity
	 *
	 * \param a_rotationQuat The new rotation quaterions of the Entity
	 */
	GEAR_API void SetRotationQuaterions(glm::quat a_rotationQuat);
	/*!
	 * Gets the rotation quaterion of the Entity
	 *
	 * \return The rotation quaterions of the Entity
	 */
	GEAR_API glm::quat GetRotationQuaterions();
	/*!
	 * Sets the rotation angles of the Entity
	 * relative to its parent
	 *
	 * \param a_rotationAngles The new rotation angles of the Entity relative to its parent
	 */
	GEAR_API void SetLocalRotationAngles(glm::vec3 a_rotationAngles);
	/*!
	 * Gets the rotation angles of the Entity
	 * relative to its parent
	 *
	 * \return The rotation angles of the Entity relative to its parent
	 */
	GEAR_API glm::vec3 GetLocalRotationAngles();
	/*!
	 * Sets the scale of the Entity
	 *
	 * \param a_scale The new scale of the Entity
	 */
	GEAR_API void SetScale(glm::vec3 a_scale);
	/*!
	 * Gets the scale of the Entity
	 *
	 * \return The scale of the Entity
	 */
	GEAR_API glm::vec3 GetScale();
	/*!
	 * Sets the scale of the Entity
	 * relative to its parent
	 *
	 * \param a_scale The new scale of the Entity relative to its parent
	 */
	GEAR_API void SetLocalScale(glm::vec3 a_scale);
	/*!
	 * Gets the scale of the Entity relative
	 * to its parent
	 *
	 * \return The scale of the Entity relative to its parent
	 */
	GEAR_API glm::vec3 GetLocalScale();
	/*!
	 * Gets the position of the Entity
	 *
	 * \return The position of the Entity
	 */
	GEAR_API std::vector<float> GetLocalTuplePosition();
	/*!
	 * Gets the scale of the Entity
	 *
	 * \return The scale of the Entity
	 */
	GEAR_API std::vector<float> GetLocalTupleScale();

	GEAR_API glm::mat4 GetRotationMatrix();

	GEAR_API glm::mat4 GetLocalRotationMatrix();

	GEAR_API void Clone(int objID);

private:
	glm::vec3 mPosition; /*!< X, y, and z Position of the Entity relative to parent Entity */
	glm::vec3 mScale; /*!< X, y, and z Scale of the Entity relative to parent Entity */
	glm::vec3 mRotationAngles; /*!< Yaw, pitch, and roll of the Entity in degrees relative to parent Entity */
	glm::quat mQuaterions;

	glm::vec3 mLocalPosition; /*!< X, y, and z Position of the Entity */
	glm::vec3 mLocalScale; /*!< X, y, and z Scale of the Entity */
	glm::vec3 mLocalRotationAngles; /*!< Yaw, pitch, and roll of the Entity in degrees */
};
