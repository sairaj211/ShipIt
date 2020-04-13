#pragma once

#define RESOURCE_MANAGER ResourceManager::GetInstance()

class JsonData;
class Texture;
class Shader;
class Material;
class Model;
class AffineAnimation;
class SkeletalAnimation;
class Text;

/*!
 * Loads and unloads resources used
 * by the engine
 * 
 */
class ResourceManager
{
public:
	/*!
	 * Get the ResourceManager Singleton
	 * reference
	 * 
	 * \return The ResourceManager reference
	 */
	GEAR_API static ResourceManager* GetInstance();
	/*!
	 * Delete the ResourceManager Singleton
	 * 
	 */
	GEAR_API void Cleanup();
	/*!
	 * Reload the JsonData from the specified Json
	 * file
	 *
	 * \param a_fileName The name of the Json file to be reloaded
	 * \param a_path The path string that leads to the specified Json file
	 */
	GEAR_API void ReloadJsonData(const std::string& a_fileName, std::string a_path = ".\\Resources\\Json\\");
	/*!
	 * Load the JsonData from the specified Json
	 * file
	 *
	 * \param a_fileName The name of the Json file to be loaded
	 * \param a_path The path string that leads to the specified Json file
	 * \return The JsonData loaded from the specified Json file
	 */
	GEAR_API JsonData* LoadJsonData(const std::string& a_fileName, std::string a_path = ".\\Resources\\Json\\");
	/*!
	 * Load the Texture from the specified png
	 * file
	 *
	 * \param a_fileName The name of the png file to be loaded
	 * \return The Texture loaded from the specified png file
	 */
	GEAR_API Texture* LoadTexture(const std::string& a_fileName);
	/*!
	 * Generate and store a 2d texture returning the texture
	 * 
	 * \param a_fileName the name of this texture, names should be unique
	 * \param a_per persistnece values for perlin noise generation
	 * \param a_scale scale for perlin noise generation
	 * \param a_freq frequency for perlin noise generation
	 * \param a_res resolution of the texture
	 * \param a_width width of the texture
	 * \param a_height height of the texture
	 * \return a Texture resource that has been saved in the resource manager by name aName
	 */
	GEAR_API Texture* GenerateTexture(const std::string& a_fileName, int a_octave, float a_per, float a_scale, float a_freq, int a_res, int a_width, int a_height);
	/*!
	 * Load the Shader from the specified glsl shader
	 * file
	 *
	 * \param a_fileName The name of the glsl shader file to be loaded
	 * \return The Shader loaded from the specified glsl shader file
	 */
	GEAR_API Shader* LoadShader(const std::string& a_fileName);
	/*!
	 * Load the Material from the specified mat 
	 * file
	 *
	 * \param a_fileName The name of the mat file to be loaded
	 * \return The Material loaded from the specified mat file
	 */
	GEAR_API Material* LoadMaterial(const std::string& a_fileName);
	/*!
	 * Free the Material corresponding to the given mat
	 * file
	 *
	 * \param a_fileName The name of the mat file to be freed
	 */
	GEAR_API void FreeMaterial(const std::string& a_fileName);
	/*!
	 * Load the Model from the specified model fbx
	 * file
	 *
	 * \param a_fileName The name of the model fbx file to be loaded
	 * \return The Model loaded from the specified model fbx file
	 */
	GEAR_API Model* LoadModel(const std::string& a_fileName);
	/*!
	 * Load the AffineAnimation from the specified aat
	 * file
	 *
	 * \param a_fileName The name of the aat file to be loaded
	 * \return The AffineAnimation loaded from the specified aat file
	 */
	GEAR_API AffineAnimation* LoadAffineAnimation(const std::string& a_fileName);
	/*!
	 * Load the SkeletalAnimation from the specified animation fbx
	 * file
	 *
	 * \param a_fileName The name of the animation fbx file to be loaded
	 * \return The SkeletalAnimation loaded from the specified animation fbx file
	 */
	GEAR_API SkeletalAnimation* LoadSkeletalAnimation(const std::string& a_fileName);
	/*!
	 * Load the Text from the specified text file
	 *
	 * \param a_fileName The name of the text file to be loaded
	 * \return The Text loaded from the specified text file
	 */
	GEAR_API Text* LoadText(const std::string& a_fileName);

private:
	GEAR_API ResourceManager();
	GEAR_API ~ResourceManager();

private:
	static ResourceManager* mInstance;
	std::unordered_map<std::string, JsonData*> mJsonData;
	std::unordered_map<std::string, Texture*> mTextures;
	std::unordered_map<std::string, Shader*> mShaders;
	std::unordered_map<std::string, Model*> mModels;
	std::unordered_map<std::string, Material*> mMaterials;
	std::unordered_map<std::string, AffineAnimation*> mAffineAnimations;
	std::unordered_map<std::string, SkeletalAnimation*> mSkeletalAnimations;
	std::unordered_map<std::string, Text*> mText;
};