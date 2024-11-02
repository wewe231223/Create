#include "pch.h"
#include "Game/gameobject/UIObject.h"

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								UI Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


UIModel::UIModel(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight{ 0,0 }, mImageUnit{ 0,0 }, mSpritable{ false }
{
	mContext.ImageIndex = imageIndex;
	UIModel::UpdateScreenTransform();
}

UIModel::UIModel(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit)
	: mUIRenderer(uiRenderer), mImageIndex(imageIndex), mImageWidthHeight(imageWidthHeight), mImageUnit(imageUnit), mSpritable{ true }
{
	mContext.ImageIndex = imageIndex;
	UIModel::UpdateScreenTransform();


	mSpriteFrameInRow = imageWidthHeight.first / imageUnit.first;
	mSpriteFrameInCol = imageWidthHeight.second / imageUnit.second;
	// Total Sprite is mSpriteFrameInRow * mSpriteFrameInCol

	mContext.UVTransform = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
}

UIModel::~UIModel()
{
}

void UIModel::ChangeImage(TextureIndex imageIndex)
{
	mImageIndex = imageIndex;
	mSpritable = false;
	mContext.ImageIndex = imageIndex;
}

void UIModel::ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit)
{
	mImageIndex = imageIndex;
	mContext.ImageIndex = imageIndex;
	mImageWidthHeight = imageWidthHeight;
	mImageUnit = imageUnit;
	mSpritable = true;
}

void UIModel::SetActiveState(bool state)
{
	mActive = state;
}

void UIModel::ToggleActiveState()
{
	mActive ? mActive = false : mActive = true;
}

void UIModel::Update()
{

}

void UIModel::Render()
{
	if (mActive) {
		mTransform._11 = mUIRect.width;
		mTransform._22 = mUIRect.height;

		mTransform._31 = mUIRect.LTx;
		mTransform._32 = mUIRect.LTy;

		if (mSpritable) {
			mContext.UVTransform._11 = 1.f / static_cast<float>(mSpriteFrameInRow);
			mContext.UVTransform._22 = 1.f / static_cast<float>(mSpriteFrameInCol);

			mContext.UVTransform._13 = static_cast<float>(mSpriteCoord.first) * mContext.UVTransform._11;
			mContext.UVTransform._23 = static_cast<float>(mSpriteCoord.second) * mContext.UVTransform._22;
		}

		
		mContext.Transform = Transpose(Multifly(mTransform, mScreenTransform));
		mUIRenderer->WriteContext(&mContext);
		
	}
}

void UIModel::UpdateScreenTransform()
{
	mScreenTransform = mUIRenderer->GetScreenTransform();
}

void UIModel::AdvanceSprite()
{
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

DirectX::XMFLOAT3X3 UIModel::Multifly(const DirectX::XMFLOAT3X3& lhs, const DirectX::XMFLOAT3X3& rhs) const
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

DirectX::XMFLOAT3X3 UIModel::Transpose(const DirectX::XMFLOAT3X3& mat) const
{
	DirectX::XMMATRIX matrix = DirectX::XMLoadFloat3x3(&mat);
	DirectX::XMMATRIX matrix1 = DirectX::XMMatrixTranspose(matrix);

	DirectX::XMFLOAT3X3 result;
	DirectX::XMStoreFloat3x3(&result, matrix1);
	
	return result;
}






//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								UI Canvas								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////




Canvas::Canvas(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<class Window> window)
{
}

Canvas::~Canvas()
{
}

void Canvas::Load()
{
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(TextureIndex imageIndex)
{
	return std::shared_ptr<UIModel>();
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit)
{
	return std::shared_ptr<UIModel>();
}
