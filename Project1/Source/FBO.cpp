#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "FBO.h"
#include "Resources/Shader.h"

FBO::FBO(int a_width, int a_height, int a_numTextures, bool a_isShadowBuffer) : mWidth(a_width), mHeight(a_height)
{
	// Generate Frame Buffer
	glGenFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);

	if (!a_isShadowBuffer) {
		// Generate Render Buffer
		glGenRenderbuffers(1, &mRenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);

		// Attach Render Buffer to Frame Buffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderBufferID);

		std::vector<GLenum> attachments;
		for (int i = 0; i < a_numTextures; ++i) {
			GLuint textureID;

			// Generate Texture
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, 0);

			// Set texture parameters;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// ...

			// Attach texture to FBO color attachment 0
			GLenum attachment = (GLenum)((int)GL_COLOR_ATTACHMENT0 + i);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureID, 0);

			// Save IDs and attachments
			attachments.push_back(attachment);
			mTextureIDs.push_back(textureID);
		}

		glDrawBuffers(GLsizei(attachments.size()), &attachments[0]);
	}
	else {
		GLuint textureID;

		// Generate Texture
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		// Set texture parameters;
		float borderColor[] = { 1.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		// ...

		mTextureIDs.push_back(textureID);

		// Attach texture to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
		
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	// Check FBO error
	int i = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (i != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO Error: Failed to generate frame buffer: " << i << std::endl;

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind Render Buffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO()
{
	glDeleteTextures(GLsizei(mTextureIDs.size()), &mTextureIDs[0]);
	glDeleteRenderbuffers(1, &mRenderBufferID);
	glDeleteFramebuffers(1, &mID);
}

void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
}

void FBO::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::BindTexture(Shader* a_pShader, std::string a_uniformName, int a_textureUnit, int a_textureIndex)
{
	// Activate texture of the specified unit
	glActiveTexture((GLenum)((int)GL_TEXTURE0 + a_textureUnit));

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, mTextureIDs[a_textureIndex]);

	// Set shader uniform
	a_pShader->SetUniform(a_uniformName, a_textureUnit);
}

void FBO::UnbindTexture()
{
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint FBO::GetID()
{
	return mID;
}

GLuint FBO::GetTextureID(int a_index)
{
	return mTextureIDs[a_index];
}

GLuint FBO::GetRenderBufferID()
{
	return mRenderBufferID;
}