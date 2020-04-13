#pragma once

#include "Resource.h"

/*!
 * Shader Resource that defines an OpenGL
 * shader program
 *
 */
class Shader : public Resource
{
public:
	/*!
	 * Shader Overloaded Constructor
	 * 
	 * \param a_name The name of the Shader Resource
	 */
	GEAR_API Shader(std::string a_name);
	/*!
	 * Shader Default Destructor
	 * 
	 */
	GEAR_API virtual ~Shader();
	/*!
	 * Load the Shader with OpenGL
	 * 
	 */
	GEAR_API void Load();
	/*!
	 * Sets the Shader to be used when
	 * drawing
	 * 
	 */
	GEAR_API void Use();
	/*!
	 * Sets a null shader to be used when
	 * drawing
	 * 
	 */
	GEAR_API void Unuse();
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given integer
	 * 
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_val The integer value that the specified uniform variable is being set to
	 */
	GEAR_API void SetUniform(std::string a_uniformName, int a_val);
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given float
	 * 
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_x The float value that the specified uniform variable is being set to
	 */
	GEAR_API void SetUniform(std::string a_uniformName, float a_x);
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given vector of floats
	 *
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_x The x value of the specified uniform variable that is being set
	 * \param a_y The y value of the specified uniform variable that is being set
	 */
	GEAR_API void SetUniform(std::string a_uniformName, float a_x, float a_y);
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given vector of floats
	 *
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_x The x value of the specified uniform variable that is being set
	 * \param a_y The y value of the specified uniform variable that is being set
	 * \param a_z The z value of the specified uniform variable that is being set
	 */
	GEAR_API void SetUniform(std::string a_uniformName, float a_x, float a_y, float a_z);
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given vector of floats
	 *
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_x The x value of the specified uniform variable that is being set
	 * \param a_y The y value of the specified uniform variable that is being set
	 * \param a_z The z value of the specified uniform variable that is being set
	 * \param a_w The w value of the specified uniform variable that is being set
	 */
	GEAR_API void SetUniform(std::string a_uniformName, float a_x, float a_y, float a_z, float a_w);
	/*!
	 * Sets the Shader's specified uniform variable to
	 * given matrix
	 *
	 * \param a_uniformName The name of the uniform variable being set
	 * \param a_val The matrix value that the specified uniform variable is being set to
	 */
	GEAR_API void SetUniform(std::string a_uniformName, glm::mat4 a_val);

private:
	/*!
	 * Load the shader file text from the specified
	 * text file
	 *
	 * \param a_shaderName The name of the shader text file
	 * \return The string of characters reprsenting the contents of the shader file
	 */
	GEAR_API std::string LoadShaderText(std::string a_shaderName);
	/*!
	 * Compile the given shader source code of the
	 * specified type
	 *
	 * \param a_shaderSource The string of characters reprsenting the contents of the shader file
	 * \param a_shaderType The type of shader being compiled
	 * \return The ID of the shader that was compiled
	 */
	GEAR_API GLuint CompileShader(std::string a_shaderSource, GLenum a_shaderType);
	/*!
	 * Link the specified shaders into a shader 
	 * program
	 *
	 * \param a_vertShader The ID of the vertex shader to be linked
	 * \param a_fragShader The ID of the fragment shader to be linked
	 * \return The ID of the shader program that was linked
	 */
	GEAR_API GLint LinkShader(GLuint a_vertShader, GLuint a_fragShader);
	/*!
	 * Get the location ID of the specified uniform
	 * variable
	 *
	 * \param a_uniformName The name of the uniform variable whose location is being requested
	 * \return The location ID of the specified uniform variable
	 */
	GEAR_API GLint GetUniformLocation(std::string a_uniformName);
	/*!
	 * Checks for OpenGL errors that came from
	 * compiling and linking the Shader
	 *
	 * \param a_shaderID The ID of the shader being checked for errors
	 * \param a_type The specific type of error that is being checked for
	 * \param a_isProgram A flag that specifies whether the given shader ID corresponds to a shader program
	 * \param a_errorString The string that is prepended to the error output string
	 * \return A flag that is true if there was an error related to OpenGL, false otherwise
	 */
	GEAR_API bool CheckShaderError(GLuint a_shaderID, GLuint a_type, bool a_isProgram, std::string a_errorString);

public:
	GLuint mShaderID; /*!< OpenGL ID of the Shader */
};