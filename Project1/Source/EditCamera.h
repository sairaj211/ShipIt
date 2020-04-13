#pragma once

/*!
 * Manages the default Camera used by
 * the engine
 *
 */
class EditCamera
{
public:
	/*!
	 * EditCamera Default Constructor
	 *
	 */
	GEAR_API EditCamera();
	/*!
	 * EditCamera Default Destructor
	 *
	 */
	GEAR_API ~EditCamera();
	/*!
	 * Update the Camera's position and orientation
	 * based on user input
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Gets the position of the Camera
	 *
	 * \returns The position of the Camera
	 */
	GEAR_API glm::vec3 GetPosition();
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
	 * Gets the near plane of the Camera
	 *
	 * \return The near plane of the Camera
	 */
	GEAR_API float GetNearPlane();
	/*!
	 * Gets the far plane of the Camera
	 *
	 * \return The far plane of the Camera
	 */
	GEAR_API float GetFarPlane();
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


private:
	GEAR_API void UpdateCameraVectors();
	GEAR_API void UpdateCameraMatrices();

private:
	glm::vec3 mPosition; /*!< Position of the Camera */
	glm::vec3 mRotationAngles; /*!< Rotation angles of the Camera */

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