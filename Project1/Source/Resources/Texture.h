#pragma once
#include "Resource.h"

class Shader;

/*!
 * Texture Resource that defines an image
 * with its OpenGL ID
 *
 */
class Texture : public Resource
{
public:
	/*!
	 * Texture Overloaded Constructor
	 * 
	 * \param a_name The name of the Texture Resource
	 */
	GEAR_API Texture(std::string a_name);
	/*!
	 * Texture Default Destructor
	 * 
	 */
	GEAR_API virtual ~Texture();
	/*!
	 * Load the Texture with OpenGL
	 * 
	 */
	GEAR_API void Load();
	/*!
	 * Generates a Perlin Noise Texture and writes
	 * the data to a png file
	 * 
	 * \param a_octave The number of Octaves for the Perlin Noise
	 * \param a_per The persistence for the Perlin Noise
	 * \param a_scale The scale for the Perlin Noise
	 * \param a_freq The frequency for the Perlin Noise
	 * \param a_res The resolution of the Perlin Noise Texture
	 * \param a_width The width of the Perlin Noise Texture
	 * \param a_height The height of the Perlin Noise Texture
	 */
	GEAR_API void LoadRandom(int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height);
	/*!
	 * Binds the specified Frame texture to the given
	 * Shader
	 * 
	 * \param a_pShader The Shader the texture is being bound to
	 * \param a_uniformName The name of the uniform variable in the shader that the texture will be bound to
	 * \param a_textureUnit The offset into the OpenGL texture units
	 */
	GEAR_API void Bind(Shader* a_pShader, std::string a_uniformName, int a_textureUnit);
	/*!
	 * Unbinds the Texture
	 * 
	 */
	GEAR_API void Unbind();

private:
	/*!
	 * Generates the Texture data according to the 
	 * Perlin Noise specified by the given parameters 
	 *
	 * \param a_octave The number of Octaves for the Perlin Noise
	 * \param a_per The persistence for the Perlin Noise
	 * \param a_scale The scale for the Perlin Noise
	 * \param a_freq The frequency for the Perlin Noise
	 * \param a_res The resolution of the Perlin Noise Texture
	 * \param a_width The width of the Perlin Noise Texture
	 * \param a_height The height of the Perlin Noise Texture
	 * \return The Texture data generated from the Perlin Noise generator
	 */
	GEAR_API std::vector<unsigned char> GenerateNoiseTextureData(int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height);

public:
	GLuint mTextureID; /*!< OpenGL ID of the Texture */
};