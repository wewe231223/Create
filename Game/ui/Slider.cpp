#include "pch.h"
#include "Game/ui/Slider.h"
#include "Game/gameobject/UIObject.h"

Slider::Slider(class Canvas* canvas, const std::string& base, const std::string& bar, POINT LT, UINT width, UINT height)
	:	mBase(canvas->CreateUIModel(base)),
		mSlider(canvas->CreateUIModel(bar))
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

void Slider::Update()
{
	mSlider->GetUIRect().width = mUnit * mValue;
}
