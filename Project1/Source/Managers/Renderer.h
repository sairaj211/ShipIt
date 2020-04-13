#pragma once

#define RENDERER Renderer::GetInstance()

class Camera;
class ModelComponent;
class DRLightComponent;
class FBO;
class Shader;
class AABBTree;

/*!
 * A Struct defining the attributes of a
 * character of text
 *
 */
struct Character
{
	GLuint mTextureID;   // ID handle of the glyph texture
	glm::ivec2 mSize;    // Size of glyph
	glm::ivec2 mBearing;  // Offset from baseline to left/top of glyph
	GLuint mAdvance;    // Horizontal offset to advance to next glyph
};

struct TextInfo 
{
	std::wstring mText;
	GLfloat mX;
	GLfloat mY; 
	GLfloat mScale; 
	float mR; 
	float mG; 
	float mB;
	float mA;
};

/*!
 * A Struct defining the attributes of an
 * object outline
 *
 */
struct Outline
{
	ModelComponent* pModelComponent; /*!< Model of the object being outlined */
	bool respectOcclusion; /*!< Flag indicating whether the outline is hidden by occluding objects */
	glm::vec3 color; /*!< Color of the outline */
};

/*!
 * Renders objects from a queue onto
 * the screen according to their material
 * properties
 *
 */
class Renderer 
{
public:
	/*!
	 * Get the Renderer Singleton reference
	 * 
	 * \return The Renderer Singleton reference
	 */
	GEAR_API static Renderer* GetInstance();
	/*!
	 * Delete the Renderer Singleton
	 * 
	 */
	GEAR_API void Cleanup();
	/*!
	 * Initialize Renderer with the necessary FBOs
	 * and Shaders
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Draw the current scene to the screen
	 * 
	 */
	GEAR_API void Draw();
	/*!
	 * Push the given ModelComponent to a queue to
	 * be rendered
	 * 
	 * \param a_pModelComponent The ModelComponent to be pushed to the render queue
	 */
	GEAR_API void PushToRenderQueue(ModelComponent* a_pModelComponent);
	/*!
	 * Push the given Outline to a queue to
	 * be rendered
	 *
	 * \param a_outline The outline to be pushed to the outline queue
	 */
	GEAR_API void PushToOutlineQueue(Outline a_outline);
	/*!
	 * Push the given ModelComponent to a queue of objects
	 * that cast a shadow
	 * 
	 * \param a_pModelComponent The ModelComponent that casts a shadow
	 */
	GEAR_API void PushToShadowRenderQueue(ModelComponent* a_pModelComponent);
	/*!
	 * Push the given ModelComponent to a queue objects that don't support
	 * Deferred Shading
	 * 
	 * \param a_pModelComponent The ModelComponent of the object that doesn't support Deferred Shading
	 */
	GEAR_API void PushToForwardRenderQueue(ModelComponent* a_pModelComponent);
	/*!
	 * Push the given ModelComponent to a queue of objects that
	 * have debug drawing enabled
	 *
	 * \param a_pModelComponent The ModelComponent of the object that has debug drawing enabled
	 */
	GEAR_API void PushToDebugRenderQueue(ModelComponent* a_pModelComponent);
	/*!
	 * Push the given ModelComponent to a queue of objects that
	 * are UI elements
	 *
	 * \param a_pModelComponent The ModelComponent of the object that are UI elements
	 */
	GEAR_API void PushToUIRenderQueue(ModelComponent* a_pModelComponent);
	/*!
	 * Loads the specified font into the vector of
	 * available text render characters
	 * 
	 * \param a_font The name of the font to be loaded
	 */
	GEAR_API void ReloadFonts(std::string a_font);
	/*!
	 * Render the specified text onto the screen at the
	 * specified location, pushes the values onto the text
	 * rendering queue to be rendered in the forward rendering
	 * 
	 * \param a_text UTF16 string to be rendered
	 * \param a_x x location on the screen in screen coordinates
	 * \param a_y y location on the screen in screen coordinates
	 * \param a_scale The scaling factor of the text
	 * \param a_r Red color value for rgba values should be clamped to 0..1
	 * \param a_g Green color value for rgba values should be clamped to 0..1
	 * \param a_b Blue color value for rgba values should be clamped to 0..1
	 * \param a_a Alpha color value for rgba values should be clamped to 0..1
	 */
	GEAR_API void RenderText(std::wstring a_text, GLfloat a_x, GLfloat a_y, GLfloat a_scale, float a_r, float a_g, float a_b, float a_a);
	/*!
	 * Tells the renderer to either fade in or out in the
	 * given amount of time
	 *
	 * \param a_fadeIn Flag indicaing whether the renderer should fade in instead of out
	 * \param a_fadeTime The time it takes for the renderer to fade
	 */
	GEAR_API void Fade(bool a_fadeIn, float a_fadeTime);
	/*!
	 * Get the multiplier used to fade in and out
	 *
	 * \return The multiplier used to fade in and out
	 */
	GEAR_API float GetFadeMultiplier();
	GEAR_API void RenderContacts();
	GEAR_API void DrawAABBTree(AABBTree* tree, glm::vec3 a_color = glm::vec3(0));

private:
	GEAR_API Renderer();
	GEAR_API ~Renderer();

	GEAR_API void ShadowRender(glm::mat4 a_viewMatrix, glm::mat4 a_projectionMatrix);
	GEAR_API void GBufferRender();
	GEAR_API void AmbientRender();
	GEAR_API void GlobalRender();
	GEAR_API void LocalRender();
	GEAR_API void PBufferRender();
	GEAR_API void ForwardRender();
	
	GEAR_API void DebugRender();
	GEAR_API void OutlineRender();
	GEAR_API void UIRender();
	
	GEAR_API void DrawAABB(ModelComponent* a_pModelComponent, glm::vec3 a_color = glm::vec3(0));
	GEAR_API void DrawVelocity(ModelComponent* a_pModelComponent);
	GEAR_API void DrawCollider(ModelComponent* a_pModelComponent);
	GEAR_API void DrawRenderableText(TextInfo a_text);

	GEAR_API glm::mat4 ComputeLightView(glm::vec3 a_lightPosition, glm::vec3 a_lightDirection);

	GEAR_API void DrawBox(glm::vec2 a_minMaxX, glm::vec2 a_minMaxY, glm::vec2 a_minMaxZ, glm::vec3 a_color = glm::vec3(0));


private:
	GEAR_API void LoadCharacter(wchar_t a_char);

public:
	bool mAntiAliasing; /*!< Flag indicating whether anti-aliasing should be used */
	bool mShowText; /*!< Flag indicating whether we render our text or not*/
private:
	static Renderer *mInstance;

	std::queue<ModelComponent*> mRenderable;
	std::queue<TextInfo> mTextQueue;
	std::queue<Outline> mOutlineQueue;
	std::queue<ModelComponent*> mUIQueue;
	std::vector<ModelComponent*> mShadowRenderable;
	std::vector<ModelComponent*> mForwardRenderable;
	std::vector<ModelComponent*> mDebugRenderable;

	FBO* mpDepthFBO;
	FBO* mpGBuffer;
	FBO* mpPBuffer;
	FBO* mpSilhouetteBuffer;

	FBO* mpBufferThread1;
	FBO* mpBufferThread2;

	Shader* mpShadowShader;
	Shader* mpLocalShader;
	Shader* mpGlobalShader;
	Shader* mpSpotShader;
	Shader* mpGBufferShader;
	Shader* mpAmbientShader;
	Shader* mpPBufferShader;
	Shader* mpForwardShader;
	Shader* mpTextShader;
	Shader* mpLineShader;
	Shader* mpOutlineShader;
	Shader* mpDefaultShader;

	Shader* mpForwardShaderThread1;
	Shader* mpForwardShaderThread2;

	GLuint mDebugVAO;
	GLuint mDebugVBO;
	GLuint mDebugEBO;

	GLuint mTextVAO;
	GLuint mTextVBO;
	GLuint mTextEBO;

	float mSpanMax;
	float mReduceMin;
	float mReduceMultiplier;

	float mFadeMultiplier;
	float mFadeTime;
	float mFadeMaxTime;
	bool mFade;
	bool mFadeIn;

	std::map<wchar_t, Character> mCharacters;
};

