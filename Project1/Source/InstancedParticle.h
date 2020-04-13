#pragma once

class Material;
struct instancedData
{
	glm::mat4 ModelMatrix;
	glm::vec4 Color;
};

/*! \class InstancedParticle
 * \brief Class definition for storing instancing data of particles and drawing(OpenGL calls)
 *
*/
class InstancedParticle
{
public:
	InstancedParticle();
	~InstancedParticle();

	bool Initialize(const char* a_MaterialName);
	void Draw();
	Material* m_Material;
	bool m_IsTexture;

	std::vector<instancedData>	m_InstancedParData;
};