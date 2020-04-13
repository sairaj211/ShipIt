#pragma once
#include "Resource.h"

class Mesh;
class Material;
class Skeleton;

/*!
 * Model Resource that defines a collection
 * of Meshes and their skeletal structure
 *
 */
class Model : public Resource
{
public:
	/*!
	 * Model Overloaded Contructor
	 * 
	 * \param a_name The name of the Model Resource
	 */
	GEAR_API Model(std::string a_name);
	/*!
	 * Model Default Destructor
	 * 
	 */
	GEAR_API virtual ~Model();
	/*!
	 * Load the Model with ASSIMP
	 * 
	 */
	GEAR_API void Load();

private:
	GEAR_API Mesh* SerializeMesh(aiMesh* a_meshData);
	GEAR_API float GetMaxLength();
	GEAR_API void UpdateMinMax(Mesh* a_pMesh, glm::vec3 a_vertex);

public:
	const aiScene* mpScene; /*!< ASSIMP scene used to load the Model */
	Skeleton* mpSkeleton; /*!< Skeleton structure of the Model */
	std::vector<Mesh*> mMeshes; /*!< Vector of the Meshes that make up the Model */
	glm::mat4 mGlobalInverse; /*!< The inverse transformation from root of the ASSIMP scene */
	glm::vec2 mMinMaxX; /*!< Minimum and maximum x value seen amoung each of the Model's Meshes */
	glm::vec2 mMinMaxY; /*!< Minimum and maximum y value seen amoung each of the Model's Meshes */
	glm::vec2 mMinMaxZ; /*!< Minimum and maximum z value seen amoung each of the Model's Meshes */
	float mMaxLength; /*!< Maximum difference in vertex position range in each of the x, y, and z directions */
};