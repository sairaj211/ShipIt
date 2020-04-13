#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Texture.h"
#include "Shader.h"
#include "Random/Noise.h"

Texture::Texture(std::string a_name) : Resource(a_name)
{

}

Texture::~Texture()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	glGenTextures(1, &mTextureID);

	if (mName.find("skybox") != mName.npos) {
		// Setup glTexture
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

		std::vector<std::string> textureNames;
		for (int i = 0; i < 6; ++i)
			textureNames.push_back(mName + std::to_string(i));

		std::vector<GLenum> targets;
		targets.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		targets.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		targets.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		targets.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);;
		targets.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		targets.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

		for (int i = 0; i < 6; ++i) {
			std::string filePath = ".\\Resources\\Textures\\" + textureNames[i] + ".png";

			// Load image data
			int height, width, channels;
			unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

			// Check image was loaded successfully
			if (!imageData) {
				LOG_ERROR("Failed to load Texture " + textureNames[i] + ": " + std::string(stbi_failure_reason()));
				mLoadFailed = true;
				return;
			}

			// Set texture data
			glTexImage2D(targets[i], 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

			// Set texture parameters
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// ...

			// Free Image
			stbi_image_free(imageData);
		}
	}
	else {
		std::string filePath = "Resources\\Textures\\" + mName + ".png";

		// Load image data
		int height, width, channels;
		unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		// Check image was loaded successfully
		if (!imageData) {
			LOG_ERROR("Failed to load Texture " + mName + ": " + std::string(stbi_failure_reason()));
			mLoadFailed = true;
			return;
		}

		// Setup glTexture
		glBindTexture(GL_TEXTURE_2D, mTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// ...

		// Free Image
		stbi_image_free(imageData);
	}

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadRandom(int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height)
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	std::vector<unsigned char> textureData = GenerateNoiseTextureData(a_octave, a_per, a_scale, a_freq, a_res, a_width, a_height);
	
	// Create Texture File for Reuse
	std::string filePath = "Resources\\Textures\\" + mName + ".png";
	stbi_write_png(filePath.c_str(), a_width, a_height, 3, textureData.data(), a_width * 3);

	// Generate Texture
	glGenTextures(1, &mTextureID);

	// Setup Texture
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_width, a_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)textureData.data());

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ...

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(Shader* a_pShader, std::string a_uniformName, int a_textureUnit)
{
	// Activate texture of the specified unit
	glActiveTexture((GLenum)((int)GL_TEXTURE0 + a_textureUnit));

	// Bind texture
	if (a_uniformName == "cubeMap")
		glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
	else
		glBindTexture(GL_TEXTURE_2D, mTextureID);

	// Set shader uniform
	a_pShader->SetUniform(a_uniformName, a_textureUnit);
}

void Texture::Unbind() 
{
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<unsigned char> Texture::GenerateNoiseTextureData(int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height)
{
	std::vector<unsigned char> textureData(a_res * 3);
	int index = 0;

	for (int i = 0; i < a_width; ++i) {
		for (int j = 0; j < a_height; ++j) {
			float noise = NOISE->PerlinNoise(a_octave, a_per, a_scale, i / a_freq, j / a_freq, 0.0f, 0.0f, 1.0f) * 255;

			textureData[index++] = unsigned char(noise);
			textureData[index++] = unsigned char(noise);
			textureData[index++] = unsigned char(noise);
		}
	}

	return textureData;
}