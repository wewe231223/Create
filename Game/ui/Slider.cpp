#include "pch.h"
#include "Game/ui/Slider.h"
#include "Game/gameobject/UIObject.h"

Slider::Slider(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex image, POINT LT, UINT width, UINT height) 
	:	mBase(std::make_unique<UIModel>(uiRenderer, image)),
		mSlider(std::make_unique<UIModel>(uiRenderer, image))
{
	mBase->GetUIRect().LTx = static_cast<float>(LT.x);
	mBase->GetUIRect().LTy = static_cast<float>(LT.y);
	mBase->GetUIRect().width = static_cast<float>(width);
	mBase->GetUIRect().height = static_cast<float>(height);


	mUnit = width / 100.f;

	mSlider->GetUIRect().LTx = static_cast<float>(LT.x);
	mSlider->GetUIRect().LTy = static_cast<float>(LT.y);
	mSlider->GetUIRect().width = mUnit * mValue;
	mSlider->GetUIRect().height = static_cast<float>(height);
}

Slider::~Slider()
{
}
