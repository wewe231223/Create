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
	mUIRenderer = std::make_shared<UIRenderer>(device, commandList, window);
}

Canvas::~Canvas()
{
}

void Canvas::Load()
{
	const fs::path folderPath = "./Resources/Images/";

	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (fs::is_regular_file(entry.status())) {
			mUIRenderer->CreateUIImage(entry.path().stem().string(), entry.path());
		}
	}
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(TextureIndex imageIndex)
{
	return mUIModels.emplace_back(std::make_shared<UIModel>(mUIRenderer, imageIndex));
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit)
{
	return mUIModels.emplace_back(std::make_shared<UIModel>(mUIRenderer, imageIndex, imageWidthHeight,imageUnit));
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(const std::string& imageName)
{
	 return mUIModels.emplace_back(std::make_shared<UIModel>(mUIRenderer, mUIRenderer->GetUIImage(imageName)));
}

std::shared_ptr<UIModel> Canvas::CreateUIModel(const std::string& imageName, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit)
{
	return mUIModels.emplace_back(std::make_shared<UIModel>(mUIRenderer, mUIRenderer->GetUIImage(imageName), imageWidthHeight, imageUnit));
}

void Canvas::Update()
{
	for (auto& uiObject : mUIObjects) {
		uiObject->Update();
	}

	for (auto& uiModel : mUIModels) {
		uiModel->Render();
	}
}

void Canvas::Render(ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	mUIRenderer->Render(commandList);
}




//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								Slider									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


Slider::Slider(class Canvas* canvas, const std::string& base, const std::string& bar, POINT LT, UINT width, UINT height)
	: mBase(canvas->CreateUIModel(base)),
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
	mValue = std::clamp(mValue, 0.f, 100.f);
	mSlider->GetUIRect().width = mUnit * mValue;
}
