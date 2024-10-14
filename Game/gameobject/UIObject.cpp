#include "pch.h"
#include "Game/gameobject/UIObject.h"

UIObject::UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight{ 0,0 }, mImageUnit{ 0,0 }, mSpritable{ false }
{
	
}

UIObject::UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, std::pair<UINT, UINT> mImageUnit)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight(imageWidthHeight), mImageUnit(mImageUnit), mSpritable{ true }
{
}

UIObject::~UIObject()
{
}

void UIObject::ChangeImage(TextureIndex imageIndex)
{
	mImageIndex = imageIndex;
	mSpritable = false;
}

void UIObject::ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, std::pair<UINT, UINT> mImageUnit)
{
	mImageIndex = imageIndex;
	mImageWidthHeight = imageWidthHeight;
	mImageUnit = mImageUnit;
	mSpritable = true;
}

void UIObject::Update()
{

}

void UIObject::Render()
{

}
