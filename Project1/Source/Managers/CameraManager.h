#pragma once

#define CAMERA_MANAGER CameraManager::GetInstance()

class Camera;
class EditCamera;

class CameraManager
{
public:
	/*!
	 * Get the CameraManager Singleton reference
	 *
	 * \return The CameraManager reference
	 */
	GEAR_API static CameraManager* GetInstance();
	/*!
	 * Nothing at the moment
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Update the default Camera if there is
	 * no current Camera
	 *
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Delete the CameraManager Singleton
	 * 
	 */
	GEAR_API void Cleanup();
	/*!
	 * Gets the position of the current Camera
	 *
	 * \returns The position of the current Camera
	 */
	GEAR_API glm::vec3 GetCameraPosition();
	/*!
	 * Get the x, y, & z position of the current Camera
	 *
	 * \return A vector of the x, y, and z position values of the current Camera
	 */
	GEAR_API std::vector<float> GetVectorCameraPosition();
	/*!
	 * Gets the front vector of the current Camera
	 *
	 * \returns The front vector of the current Camera
	 */
	GEAR_API glm::vec3 GetCameraFront();
	/*!
	 * Get the x, y, & z front vector values of the current Camera
	 *
	 * \return A vector of the x, y, & z front vector values of the current Camera
	 */
	GEAR_API std::vector<float> GetVectorCameraFront();
	/*!
	 * Gets the up vector of the current Camera
	 *
	 * \returns The up vector of the current Camera
	 */
	GEAR_API glm::vec3 GetCameraUp();
	/*!
	 * Gets the right vector of the current Camera
	 *
	 * \returns The right vector of the current Camera
	 */
	GEAR_API glm::vec3 GetCameraRight();
	/*!
	 * Gets the near plane of the current Camera
	 *
	 * \return The near plane of the current Camera
	 */
	GEAR_API float GetCameraNearPlane();
	/*!
	 * Gets the far plane of the current Camera
	 *
	 * \return The far plane of the current Camera
	 */
	GEAR_API float GetCameraFarPlane();
	/*!
	 * Gets the field of view of the current Camera
	 *
	 * \return The field of view of the current Camera
	 */
	GEAR_API float GetCameraFieldOfView();
	/*!
	 * Gets the aspect ratio of the current Camera
	 *
	 * \return The aspect ratio of the current Camera
	 */
	GEAR_API float GetCameraAspectRatio();
	/*!
	 * Gets the view matrix of the current Camera
	 *
	 * \return The view matrix of the current Camera
	 */
	GEAR_API glm::mat4 GetCameraViewMatrix();
	/*!
	 * Gets the projection matrix of the current Camera
	 *
	 * \return The projection matrix of the current Camera
	 */
	GEAR_API glm::mat4 GetCameraProjectionMatrix();
	/*!
	 * Sets the current Camera to given Camera
	 * 
	 * \param a_camera The new Camera
	 */
	GEAR_API void SetCurrentCamera(Camera* a_camera);
	/*!
	 * Gets the current Camera to ID
	 *
	 * \return The ID of the current Camera
	 */
	GEAR_API int GetCameraID();
	
	GEAR_API glm::mat4 GetCameraOrthogonalMatrix();

private:
	GEAR_API CameraManager();
	GEAR_API ~CameraManager();

private:
	bool mCaptureInput;
	Camera* mpCurrentCamera;
	EditCamera* mpDefaultCamera;
	static CameraManager* mInstance;
};