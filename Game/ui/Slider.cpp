#include "pch.h"
#include "Game/ui/Slider.h"
#include "Game/gameobject/UIObject.h"

Slider::Slider(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex image, POINT LT, UINT width, UINT height) 
	:	mBase(std::make_unique<UIModel>(uiRenderer, image)),
		mSlider(std::make_unique<UIModel>(uiRenderer, image))
{
	mBase->GetUIRect().LTx = LT.x;
	mBase->GetUIRect().LTy = LT.y;
	mBase->GetUIRect().width = width;
	mBase->GetUIRect().height = height;


	mUnit = width / 100.f;

	mSlider->GetUIRect().LTx = LT.x;
	mSlider->GetUIRect().LTy = LT.y;
	mSlider->GetUIRect().width = mUnit * mValue;
	mSlider->GetUIRect().height = height;
}

Slider::~Slider()
{
}
