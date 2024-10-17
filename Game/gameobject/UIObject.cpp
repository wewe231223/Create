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

/*
float _11, _12, _13;
float _21, _22, _23;
float _31, _32, _33;
*/

void UIObject::Render()
{
	mTransform._11 = mUIRect.width;
	mTransform._22 = mUIRect.height;

	mTransform._31 = mUIRect.LTx;
	mTransform._32 = mUIRect.LTy;

	// TODO : 이건 좀 나중에..
	if (mSpritable) {

	}
	else {

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
