#pragma once

/*!
 * A Struct defining the attributes and
 * functions of the vertices that make
 * up a Mesh
 * 
 */
struct Vertex
{
	glm::vec3 position; /*!< Position of the Vertex */
	glm::vec3 normal; /*!< Normal of the Vertex */
	glm::vec2 textureCoordinate; /*!< Texture Coordinates of the Vertex */
	glm::vec3 tangent; /*!< tangent of the triangle associated with this vertex */
	unsigned int boneIndices[4]; /*!< Array of indices corresponding to the Bones associated with the Vertex */
	float boneWeights[4]; /*!< Array of weights given to each Bone associated with the Vertex */

	/*!
	 * Vertex Default Constructor
	 *
	 */
	Vertex();
	/*!
	 * Fills an open slot in the boneIndices and boneWeights 
	 * arrays with the given data 
	 *
	 * \param a_index The index of the Bone being associated with the vertex
	 * \param a_weight The weight of the Bone being associated with the vertex
	 */
	void AddBone(unsigned int a_index, float a_weight);
};

/*!
 * A collection of vertices and indices that
 * define 3D objects
 *
 */
class Mesh
{
public:
	/*!
	 * Mesh Default Constructor
	 * 
	 */
	GEAR_API Mesh();
	/*!
	 * Mesh Default Destructor
	 * 
	 */
	GEAR_API ~Mesh();
	/*!
	 * Fills its VAO, VBO, and EBO with the
	 * current Vertex data 
	 * 
	 */
	GEAR_API void Initialize();
	/*!
	 * Normalizes the Mesh verties according
	 * to the given max length so that they are
	 * in the range [0,1]
	 *
	 * \param a_maxLength The maximum difference in vertex position range in each of the x, y, and z directions
	 */
	GEAR_API void NormalizeVertices(float a_maxLength);

public:
	std::string mMeshName; /*!< Name of the Mesh */
	std::vector<Vertex> mVertices; /*!< Vector of the vertices of the Mesh */
	std::vector<GLuint> mIndices; /*!< Vector of the Vertex Indices for the Mesh*/
	GLuint mVAO; /*!< Vertex Array Object of the Mesh */
	GLuint mVBO; /*!< Vertex Buffer Object of the Mesh */
	GLuint mInsatnceVBO; /*!< Vertex Buffer Object of the Mesh */
	GLuint mEBO; /*!< Element Buffer Object of the Mesh */
	glm::vec2 mMinMaxX; /*!< The minimum and maximum Vertex values on the x axis (in that order) */
	glm::vec2 mMinMaxY; /*!< The minimum and maximum Vertex values on the y axis (in that order) */
	glm::vec2 mMinMaxZ; /*!< The minimum and maximum Vertex values on the z axis (in that order) */
};