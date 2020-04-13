#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "Utility.h"
#include "Managers/ComponentManager.h"
#include "Managers/CameraManager.h"
#include "Components/ModelComponent.h"
#include "Components/Transform.h"
#include "Engine.h"
#include "Random/RandomNumberGenerator.h"

glm::mat4 AIMatrixToGLM(aiMatrix4x4 a_matrix)
{
	glm::mat4 result;

	result[0][0] = a_matrix.a1;
	result[1][0] = a_matrix.a2;
	result[2][0] = a_matrix.a3;
	result[3][0] = a_matrix.a4;

	result[0][1] = a_matrix.b1;
	result[1][1] = a_matrix.b2;
	result[2][1] = a_matrix.b3;
	result[3][1] = a_matrix.b4;

	result[0][2] = a_matrix.c1;
	result[1][2] = a_matrix.c2;
	result[2][2] = a_matrix.c3;
	result[3][2] = a_matrix.c4;

	result[0][3] = a_matrix.d1;
	result[1][3] = a_matrix.d2;
	result[2][3] = a_matrix.d3;
	result[3][3] = a_matrix.d4;

	return result;
}

glm::vec3 AIVectorToGLM(aiVector3D a_vector)
{
	return glm::vec3(a_vector.x, a_vector.y, a_vector.z);
}

glm::quat AIQuaternionToGLM(aiQuaternion a_quaternion)
{
	return glm::quat(a_quaternion.w, a_quaternion.x, a_quaternion.y, a_quaternion.z);
}

std::vector<std::string> GetUniqueDirectoryFiles(const std::string& a_directoryPath)
{
	std::vector<std::string> uniqueFileNames;

	std::filesystem::directory_iterator itr = std::filesystem::directory_iterator(a_directoryPath);
	// For each entry in the directory, add the file name without the 
	// extension to the list of unique file names if it doesn't already
	// exist
	for (std::filesystem::directory_entry directoryEntry : itr) {
		std::filesystem::path filePath(directoryEntry);
		std::string fileName = filePath.filename().string();

		// Get the file name without the extension
		fileName = std::string(&fileName[0], &fileName[fileName.find(".")]);

		// Push the file name into the list of unique file names if it doesn't already exist
		if (std::find(uniqueFileNames.begin(), uniqueFileNames.end(), fileName) == uniqueFileNames.end())
			uniqueFileNames.push_back(fileName);
	}

	return uniqueFileNames;
}

bool CompareDistance(ModelComponent* a_pModel0, ModelComponent* a_pModel1)
{
	glm::vec3 cameraPosition = CAMERA_MANAGER->GetCameraPosition();

	Transform* pTr0 = ComponentManager::GetInstance()->GetComponent<Transform>(a_pModel0->GetOwnerID(), "Transform");
	Transform* pTr1 = ComponentManager::GetInstance()->GetComponent<Transform>(a_pModel1->GetOwnerID(), "Transform");

	glm::vec3 diff0 = pTr0->GetPosition() - cameraPosition;
	glm::vec3 diff1 = pTr1->GetPosition() - cameraPosition;

	// Compare distance between object and camera
	return glm::length(diff0) > glm::length(diff1);
}

std::wstring ToWString(std::string &aChars)
{
	std::string chars = "L";
	chars.append(aChars);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(chars);
}

std::string DecodeResource(int a_gameInt)
{
	std::string resourceString = "";

	switch (a_gameInt) {
	case 0: resourceString = "ProgrammingResource"; break;
	case 1: resourceString = "ArtResource"; break;
	case 2: resourceString = "DesignResource"; break;
	case 3: resourceString = "PhysicsResource"; break;
	case 4: resourceString = "AudioResource"; break;
	case 5: resourceString = "AIResource"; break;
	case 6: resourceString = "GraphicsResource"; break;
	default: resourceString = "";
	}

	return resourceString;
}

std::string DecodeGame(int a_gameInt)
{
	std::string gameString = "";

	switch (a_gameInt) {
	case 0: gameString = "PongGame"; break;
	case 1: gameString = "BomberGame"; break;
	case 2: gameString = "TetrisGame"; break;
	case 3: gameString = "AlienGame"; break;
	case 4: gameString = "MazeGame"; break;
	case 5: gameString = "ScienceGame"; break;
	case 6: gameString = "PoolGame"; break;
	case 7: gameString = "FoodGame"; break;
	case 8: gameString = "DiceGame"; break;
	case 9: gameString = "SpaceGame"; break;
	case 10: gameString = "CoinGame"; break;
	case 11: gameString = "LunkGame"; break;
	case 12: gameString = "KeyGame"; break;
	case 13: gameString = "NinjaGame"; break;
	case 14: gameString = "StarGame"; break;
	case 15: gameString = "BirdGame"; break;
	case 16: gameString = "SimulationGame"; break;
	case 17: gameString = "SwordGame"; break;
	case 18: gameString = "FighterGame"; break;
	case 19: gameString = "HorrorGame"; break;
	case 20: gameString = "PipeGame"; break;
	case 21: gameString = "ZombieGame"; break;
	case 22: gameString = "PortalGame"; break;
	case 23: gameString = "DragonGame"; break;
	case 24: gameString = "WarGame"; break;
	default: gameString = "";
	}

	return gameString;
}