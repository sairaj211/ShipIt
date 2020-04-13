#pragma once

class Shader;

/*!
 * A Buffer Object for managing multiple
 * Frames
 *
 */
class FBO
{
public:
	/*!
	 * FBO Overloaded Constructor
	 * 
	 * \param a_width The width of the Frames textures in the Buffer
	 * \param a_height The height of the Frames textures in the Buffer
	 * \param a_numTextures The number of Frames in the Buffer
	 * \param a_isShadowBuffer A flag that indicates whether or not the FBO being created is a ShadowBuffer
	 */
	GEAR_API FBO(int a_width, int a_height, int a_numTextures, bool a_isShadowBuffer);
	/*!
	 * FBO Default Destructor
	 * 
	 */
	GEAR_API ~FBO();
	/*!
	 * Binds the Frame Buffer Object to the 
	 * current context
	 * 
	 */
	GEAR_API void Bind();
	/*!
	 * Unbinds the Frame Buffer Object from
	 * the current context
	 * 
	 */
	GEAR_API void Unbind();
	/*!
	 * Binds the specified Frame texture to the given
	 * Shader
	 * 
	 * \param a_pShader The Shader the texture is being bound to
	 * \param a_uniformName The name of the uniform variable in the shader that the texture will be bound to
	 * \param a_textureUnit The offset into the OpenGL texture units
	 * \param a_textureIndex The Frame texture to be bound
	 */
	GEAR_API void BindTexture(Shader* a_pShader, std::string a_uniformName, int a_textureUnit, int a_textureIndex);
	/*!
	 * Unbinds the Frame textures
	 * 
	 */
	GEAR_API void UnbindTexture();
	/*!
	 * Gets the ID of the Frame Buffer Object
	 * 
	 * \return The ID of the Frame Buffer Object
	 */
	GEAR_API GLuint GetID();
	/*!
	 * Gets the texture ID of the Frame Texture
	 * at the specified index
	 * 
	 * \param a_index The index of the texture to be returned
	 * \return The texture ID of the Frame Texture at the specified index
	 */
	GEAR_API GLuint GetTextureID(int a_index);
	/*!
	 * Gets the ID of the Render Buffer associated
	 * with the Frame Buffer Object
	 * 
	 * \return The ID of the associated Render Buffer
	 */
	GEAR_API GLuint GetRenderBufferID();

public:
	GLuint mRenderBufferID;  /*!< ID of the Frame Buffer Object's Render Buffer */
	std::vector<GLuint> mTextureIDs; /*!< Vector of texture IDs for the Frame Buffer Object's Frame textures */
	GLuint mID;  /*!< ID of the Frame Buffer Object */
	int mWidth; /*!< Width of the Frame textures in the Frame Buffer Object */
	int mHeight; /*!< Height of the Frame Textures in the Frame Buffer Object */
};