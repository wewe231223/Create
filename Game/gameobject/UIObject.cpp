#include "pch.h"
#include "Game/gameobject/UIObject.h"

UIObject::UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight{ 0,0 }, mImageUnit{ 0,0 }, mSpritable{ false }
{
	mContext.ImageIndex = imageIndex;
	UIObject::UpdateScreenTransform();
}

UIObject::UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight(imageWidthHeight), mImageUnit(imageUnit), mSpritable{ true }
{
	mContext.ImageIndex = imageIndex;
	UIObject::UpdateScreenTransform();


	mSpriteFrameInRow = imageWidthHeight.first / imageUnit.first;
	mSpriteFrameInCol = imageWidthHeight.second / imageUnit.second;
	// Total Sprite is mSpriteFrameInRow * mSpriteFrameInCol

	mContext.UVTransform = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
}

UIObject::~UIObject()
{
}

void UIObject::ChangeImage(TextureIndex imageIndex)
{
	mImageIndex = imageIndex;
	mSpritable = false;
	mContext.ImageIndex = imageIndex;
}

void UIObject::ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit)
{
	mImageIndex = imageIndex;
	mContext.ImageIndex = imageIndex;
	mImageWidthHeight = imageWidthHeight;
	mImageUnit = imageUnit;
	mSpritable = true;
}



void UIObject::Disable()
{
	mActive = false;
}

void UIObject::Enable()
{
	mActive = true;
}

void UIObject::Update()
{

}

void UIObject::Render()
{
	mTransform._11 = mUIRect.width;
	mTransform._22 = mUIRect.height;

	mTransform._31 = mUIRect.LTx;
	mTransform._32 = mUIRect.LTy;

	// TODO : 이건 좀 나중에..
	if (mSpritable) {
		mContext.UVTransform._11 = 1.f / static_cast<float>(mSpriteFrameInRow);
		mContext.UVTransform._22 = 1.f / static_cast<float>(mSpriteFrameInCol);

		mContext.UVTransform._13 = static_cast<float>(mSpriteCoord.first) * mContext.UVTransform._11;
		mContext.UVTransform._23 = static_cast<float>(mSpriteCoord.second) * mContext.UVTransform._22;
	}

	if (mActive) {
		mContext.Transform = Transpose(Multifly(mTransform, mScreenTransform));
		mUIRenderer->WriteContext(&mContext);
	}
}

void UIObject::UpdateScreenTransform()
{
	mScreenTransform = mUIRenderer->GetScreenTransform();
}

void UIObject::AdvanceSprite()
{
	Console.InfoLog("AdvanceSprite");
	if (mSpritable) {
		mSpriteCoord.first++;
		if (mSpriteCoord.first >= mSpriteFrameInRow) {
			mSpriteCoord.first -= mSpriteFrameInRow;
			mSpriteCoord.second += 1;
			if (mSpriteCoord.second >= mSpriteFrameInCol) {
				mSpriteCoord.second = 0;
			}
		}
	}
}

DirectX::XMFLOAT3X3 UIObject::Multifly(const DirectX::XMFLOAT3X3& lhs, const DirectX::XMFLOAT3X3& rhs) const
{
	// XMFLOAT3X3를 XMMATRIX로 변환
	DirectX::XMMATRIX matrix1 = DirectX::XMLoadFloat3x3(&lhs);
	DirectX::XMMATRIX matrix2 = DirectX::XMLoadFloat3x3(&rhs);

	// 행렬 곱셈
	DirectX::XMMATRIX resultMatrix = DirectX::XMMatrixMultiply(matrix1, matrix2);

	// 결과를 XMFLOAT3X3로 변환
	DirectX::XMFLOAT3X3 result;
	DirectX::XMStoreFloat3x3(&result, resultMatrix);

	return result;
}

DirectX::XMFLOAT3X3 UIObject::Transpose(const DirectX::XMFLOAT3X3& mat) const
{
	DirectX::XMMATRIX matrix = DirectX::XMLoadFloat3x3(&mat);
	DirectX::XMMATRIX matrix1 = DirectX::XMMatrixTranspose(matrix);

	DirectX::XMFLOAT3X3 result;
	DirectX::XMStoreFloat3x3(&result, matrix1);
	
	return result;
}
