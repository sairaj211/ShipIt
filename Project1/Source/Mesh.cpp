#include"pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Mesh.h"

Vertex::Vertex()
{
	position = glm::vec3(0.0f);
	normal = glm::vec3(0.0f);
	textureCoordinate = glm::vec2(0.0f);
	tangent = glm::vec3(0.0f);
	memset(boneIndices, 0, sizeof(boneIndices));
	memset(boneWeights, 0, sizeof(boneWeights));
}

void Vertex::AddBone(unsigned int a_index, float a_weight)
{
	for (int i = 0; i < 4; ++i) {
		if (boneWeights[i] == 0.0f) {
			boneIndices[i] = a_index;
			boneWeights[i] = a_weight;
			return;
		}
	}
}

Mesh::Mesh()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	mMinMaxX = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
	mMinMaxY = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
	mMinMaxZ = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
}

void Mesh::Initialize()
{
	// Initialize mesh VAO
	glBindVertexArray(mVAO);

	// Fill VBO with Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	// Fill EBO with index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	// Define the attributes of the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinate));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIndices));

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));

	glBindVertexArray(0);
}

void Mesh::NormalizeVertices(float a_maxLength)
{
	// Normalize mesh AABB lengths
	mMinMaxX /= a_maxLength;
	mMinMaxY /= a_maxLength;
	mMinMaxZ /= a_maxLength;

	// Normalize vertex positions so AABB lengths are in range [0,1]
	for (int i = 0; i < mVertices.size(); ++i)
		mVertices[i].position /= a_maxLength;
}