#pragma once

class ModelComponent;

/*!
 * Converts the given ASSIMP matrix to an identical
 * glm matrix
 *
 * \param a_matrix An ASSIMP matrix
 * \return The glm matrix equivalent of the input matrix
 */
GEAR_API glm::mat4 AIMatrixToGLM(aiMatrix4x4 a_matrix);
/*!
 * Converts the given ASSIMP vector to an identical
 * glm vector
 *
 * \param a_vector An ASSIMP vector
 * \return The glm vector equivalent of the input vector
 */
GEAR_API glm::vec3 AIVectorToGLM(aiVector3D a_vector);
/*!
 * Converts the given ASSIMP quaternion to an identical
 * glm quaternion
 *
 * \param a_quaternion An ASSIMP quaternion
 * \return The glm quaternion equivalent of the input quaternion
 */
GEAR_API glm::quat AIQuaternionToGLM(aiQuaternion a_quaternion);
/*!
 * Gets a vector of the unique names of the files in
 * the specified directory
 *
 * \param a_directoryPath The string path of the path being queried
 * \return A vector of the unique names of the files in the specified directory
 */
GEAR_API std::vector<std::string> GetUniqueDirectoryFiles(const std::string& a_directoryPath);
/*!
 * A comparison function used to sort models
 * by their distance from the camera.  The
 * input ModelComponents are compared such that
 * it returns true if the distance between the camera
 * and a_pModel0 is greater than the distance between
 * camera and a_pModel1
 *
 * \param a_pModel0 The first ModelComponent to be compared
 * \param a_pModel1 The second ModelComponent to be compared
 * \return A flag indicating if the distance from the camera of the first model
 * is greater than the distance from the camera of the second model
 */
GEAR_API bool CompareDistance(ModelComponent* a_pModel0, ModelComponent* a_pModel1);
/*!
 * Convert non UTF16 charater array to a wstring
 *
 * \param aChars non UTF-16 chars to convert
 * \return UTF16 converted wstring of aChars
 */
GEAR_API std::wstring ToWString(std::string &aChars);
/*!
 * Maps the given integer to a resource string
 *
 * \param a_gameInt An integer representing a resource
 * \return The string mapped to the given integer
 */
GEAR_API std::string DecodeResource(int a_gameInt);
/*!
 * Maps the given integer to a game string
 *
 * \param a_gameInt An integer representing a game
 * \return The string mapped to the given integer
 */
GEAR_API std::string DecodeGame(int a_gameInt);