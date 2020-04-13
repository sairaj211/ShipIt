#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Shader.h"

Shader::Shader(std::string a_name) : Resource(a_name)
{

}

Shader::~Shader()
{
	glDeleteProgram(mShaderID);
}

void Shader::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	// Load Shader texts into Strings
	std::string vertString = LoadShaderText(mName + ".vert");
	std::string fragString = LoadShaderText(mName + ".frag");

	// Compile Vertex Shader
	GLuint vertShader = CompileShader(vertString, GL_VERTEX_SHADER);

	// Check for compile errors
	if (CheckShaderError(vertShader, GL_COMPILE_STATUS, false, "Vertex Shader Error: ")) {
		mLoadFailed = true;
		return;
	}

	// Compile Fragment Shader
	GLuint fragShader = CompileShader(fragString, GL_FRAGMENT_SHADER);

	// Check for compile errors
	if (CheckShaderError(fragShader, GL_COMPILE_STATUS, false, "Fragment Shader Error: ")) {
		mLoadFailed = true;
		return;
	}
	
	// Link shader program
	GLuint shaderID = LinkShader(vertShader, fragShader);

	// Check for linking error
	if (CheckShaderError(shaderID, GL_LINK_STATUS, true, "Linking Error: ")) {
		mLoadFailed = true;
		return;
	}

	mShaderID = shaderID;
}

void Shader::Use()
{
	glUseProgram(mShaderID);
}

void Shader::Unuse()
{
	glUseProgram(0);
}

void Shader::SetUniform(std::string a_uniformName, int a_val)
{
	glUniform1i(GetUniformLocation(a_uniformName), a_val);
}

void Shader::SetUniform(std::string a_uniformName, float a_x)
{
	glUniform1f(GetUniformLocation(a_uniformName), a_x);
}

void Shader::SetUniform(std::string a_uniformName, float a_x, float a_y)
{
	glUniform2f(GetUniformLocation(a_uniformName), a_x, a_y);
}

void Shader::SetUniform(std::string a_uniformName, float a_x, float a_y, float a_z)
{
	glUniform3f(GetUniformLocation(a_uniformName), a_x, a_y, a_z);
}

void Shader::SetUniform(std::string a_uniformName, float a_x, float a_y, float a_z, float a_w)
{
	glUniform4f(GetUniformLocation(a_uniformName), a_x, a_y, a_z, a_w);
}

void Shader::SetUniform(std::string a_uniformName, glm::mat4 a_val)
{	
	glUniformMatrix4fv(GetUniformLocation(a_uniformName), 1, GL_FALSE, glm::value_ptr(a_val));
}

std::string Shader::LoadShaderText(std::string a_shaderName)
{
	// Load Shader text into String
	std::ifstream file("Resources\\Shaders\\" + a_shaderName);
	std::string fileString;
	std::getline(file, fileString, (char)EOF);

	return fileString;
}

GLuint Shader::CompileShader(std::string a_shaderSource, GLenum a_shaderType)
{
	// Create shader
	GLuint shader = glCreateShader(a_shaderType);
	const GLchar* sourceText = a_shaderSource.c_str();
	glShaderSource(shader, 1, &sourceText, 0);
	
	// Compile Shader
	glCompileShader(shader);

	return shader;
}

GLint Shader::LinkShader(GLuint a_vertShader, GLuint a_fragShader)
{
	// Create Shader program
	GLint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, a_vertShader);
	glAttachShader(shaderProgram, a_fragShader);
	glLinkProgram(shaderProgram);

	// Cleanup shaders from GPU
	glDeleteShader(a_fragShader);
	glDeleteShader(a_vertShader);

	return shaderProgram;
}

GLint Shader::GetUniformLocation(std::string a_uniformName)
{
	return glGetUniformLocation(mShaderID, a_uniformName.c_str());
}

bool Shader::CheckShaderError(GLuint a_shaderID, GLuint a_type, bool a_isProgram, std::string a_errorString)
{
	bool status = false;
	GLint compileStatus;
	GLchar errorBuffer[1024] = { 0 };

	// Get compile status
	if (a_isProgram)
		glGetProgramiv(a_shaderID, a_type, &compileStatus);
	else
		glGetShaderiv(a_shaderID, a_type, &compileStatus);
	
	// Check if compilation failed
	if (compileStatus == GL_FALSE) {
		if (a_isProgram)
			glGetProgramInfoLog(a_shaderID, sizeof(errorBuffer), 0, errorBuffer);
		else
			glGetShaderInfoLog(a_shaderID, sizeof(errorBuffer), 0, errorBuffer);
		LOG_ERROR(a_errorString + std::string(errorBuffer));

		status = true;
	}

	return status;
}