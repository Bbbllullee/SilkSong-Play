#pragma once
#include "RichTextUI.h"
#include "Components/Animator.h"


class VolumeUI :public UserInterface
{
	RichTextUI* Title;
	Image* Warning;
	Animation idle;

	Button* Back;
	RichTextUI* BackText;

	class VolumeBarUI* volumeBarUI[2];
public:
	VolumeUI();

	virtual void Update(float deltaTime)override;
};