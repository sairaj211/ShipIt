#pragma once
#include "Component.h"
#include "Transform.h"

/*!
 * Manages the view and projection matrices and
 * their corresponding attributes for a Camera
 *
 */
class Camera : public Component
{
public:
	/*!
	 * Camera Default Constructor
	 *
	 */
	GEAR_API Camera();
	/*!
	 * Camera Default Destructor
	 *
	 */
	GEAR_API ~Camera();
	/*!
	 * Resets the Processor to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Initializes the Camera's attributes
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Creates a pointer to a new Camera
	 *
	 * \return A pointer to a new Camera
	 */
	GEAR_API virtual Camera* Create();
	/*!
	 * Update the Camera's attributes
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Camera so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the Camera with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Camera data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the Camera data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Camera data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 * Sets the flag indicating whether the Camera is
	 * default or not
	 *
	 * \param a_isDefault The new isDefault flag
	 */
	GEAR_API void SetIsDefault(bool a_isDefault);
	/*!
	 * Gets the flag indicating whether the Camera is
	 * default or not
	 *
	 * \return The flag indicating whether the Camera is default or not
	 */
	GEAR_API bool IsDefault();
	/*!
	 * Gets the front vector of the Camera
	 *
	 * \returns The front vector of the Camera
	 */
	GEAR_API glm::vec3 GetFront();
	/*!
	 * Gets the up vector of the Camera
	 *
	 * \returns The up vector of the Camera
	 */
	GEAR_API glm::vec3 GetUp();
	/*!
	 * Gets the right vector of the Camera
	 *
	 * \returns The right vector of the Camera
	 */
	GEAR_API glm::vec3 GetRight();
	/*!
	 * Sets the near plane of the Camera
	 *
	 * \param a_nearPlane The new near plane of the Camera
	 */
	GEAR_API void SetNearPlane(float a_nearPlane);
	/*!
	 * Gets the near plane of the Camera
	 *
	 * \return The near plane of the Camera
	 */
	GEAR_API float GetNearPlane();
	/*!
	 * Sets the far plane of the Camera
	 *
	 * \param a_farPlane The new far plane of the Camera
	 */
	GEAR_API void SetFarPlane(float a_farPlane);
	/*!
	 * Gets the far plane of the Camera
	 *
	 * \return The far plane of the Camera
	 */
	GEAR_API float GetFarPlane();
	/*!
	 * Sets the field of view of the Camera
	 *
	 * \param a_fieldOfView The new field of view of the Camera
	 */
	GEAR_API void SetFieldOfView(float a_fieldOfView);
	/*!
	 * Gets the field of view of the Camera
	 *
	 * \return The field of view of the Camera
	 */
	GEAR_API float GetFieldOfView();
	/*!
	 * Gets the aspect ratio of the Camera
	 *
	 * \return The aspect ratio of the Camera
	 */
	GEAR_API float GetAspectRatio();
	/*!
	 * Gets the view matrix of the Camera
	 *
	 * \return The view matrix of the Camera
	 */
	GEAR_API glm::mat4 GetViewMatrix();
	/*!
	 * Gets the projection matrix of the Camera
	 *
	 * \return The projection matrix of the Camera
	 */
	GEAR_API glm::mat4 GetProjectionMatrix();

	GEAR_API glm::mat4 GetOrthogonalMatrix();

	GEAR_API void Clone(int objID);

private:
	GEAR_API void UpdateCameraAttributes();

private:
	bool mIsDefault; /*!< Flag indicating whether the Camera is the default or not */
	glm::vec3 mFront; /*!< Front vector of the Camera */
	glm::vec3 mUp; /*!< Up vector of the Camera */
	glm::vec3 mRight; /*!< Right vector for the Camera */

	float mNearPlane; /*!< Near plane of the Camera */
	float mFarPlane; /*!< Far plane of the Camera */
	float mFieldOfView; /*!< Field of view of the Camera */
	float mAspectRatio; /*!< Aspect ratio of the Camera */

	glm::mat4 mViewMatrix; /*!< View matrix of the Camera */
	glm::mat4 mProjectionMatrix; /*!< Projection matrix of the Camera */
	glm::mat4 mOrthogonalMatrix; /*!< Projection matrix of the Camera */
};

