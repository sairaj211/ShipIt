#pragma once

#include "Component.h"

enum UIType
{
	INVALID = -1,
	IMAGE,
	BUTTON,
	RADIOBUTTON,
	SLIDER,
	TEXT
};

struct UIData
{
	UIType sType;
	glm::vec3 sTransform;
	glm::vec3 sScale;
	std::string sButtonLabel;
	std::string sButtonAction;
	std::string sRadioButtonLabel;
	std::vector<std::string> sRadioButtonOptions;
	std::string sSliderLabel;
	std::string sTextString;
	int sRadioButtonSelected;
	int sSliderValue;
	int sMinNum, sMaxNum;
	std::string sImageDescription;
	UIData() {
		sType = INVALID, sTransform = glm::vec3(0), sScale = glm::vec3(0), sMinNum = 0, sMaxNum = 0; sRadioButtonSelected = 0; }
};

class UIComponent : public Component
{
public:
	GEAR_API UIComponent();
	GEAR_API ~UIComponent() override;

	GEAR_API void Init() override;
	GEAR_API void Reset() override;
	GEAR_API UIComponent* Create() override;
	GEAR_API void Update(float a_deltaTime) override;
	GEAR_API void DisplayDebug() override {}
	GEAR_API void HandleEvent(Event* a_pEvent) override;
	GEAR_API void Serialize(Json::Value a_root) override;
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false) override{}
	GEAR_API void Clone(int objID) override;

private:
	void SetupLevel(std::string a_levelName);

public:
	UIData mData;
	float mTimer, mInitialTime;
	float mScaleFactor;
	glm::vec3 mSliderPosition;


	//Test
	std::string originalLabel;
	std::string tempLabel;
	std::string mLevelName;
	int mHighScore;
	bool mIsLevelButton;
	bool mIsUnlocked;
	int mLevelNumber;
	int mUnlockScore;
};