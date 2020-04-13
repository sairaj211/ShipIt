#pragma once
#include "Resource.h"

class Texture;
class Shader;

/*!
 * Material Resource that defines
 * the visual properties of an object
 * 
 */
class Material : public Resource {
public:
	/*!
	 * Material Overloaded Constructor
	 * 
	 * \param a_name The name of the Material Resource
	 */
	GEAR_API Material(std::string a_name);
	/*!
	 * Material Default Destructor
	 * 
	 */
	GEAR_API virtual ~Material();
	/*!
	 * Load the Material from Json
	 * 
	 */
	GEAR_API void Load();
	/*!
	 * Get the r, g, & b values of the diffuse
	 * color
	 *
	 * \return A vector of the r, g, and b values of the diffuse color
	 */
	GEAR_API std::vector<float> GetTupleDiffuseColor();

public:
	Json::Value mRoot;/*!< Root node of the Json hierarchy that defines the Material */
	Shader* mpShader;/*!< Shader that should be used with this Material */
	Texture* mpDiffuseTexture;/*!< Texture that defines the diffuse colors of the Material */
	Texture* mpNormalMap;/*!< Texture that defines the normals of the Material */
	glm::vec4 mDiffuseColor;/*!< Diffuse color of the Material */
	glm::vec3 mSpecularColor;/*!< Specular color of the Material */
	float mShininess;/*!< Shininess of the Material */
	bool mAdditiveBlending;/*!< Flag that specifies whether additive blending is used for this Material */
	bool mUseLighting;/*!< Flag that specifies whether lighting calculations should be computed for this Material */
};