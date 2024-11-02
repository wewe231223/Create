#pragma once 

class UIModel {
public:
	struct UIRect {
		float LTx = 0.f;
		float LTy = 0.f;
		float width = 1.f;
		float height = 1.f;
	};
public:
	UIModel(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex);
	UIModel(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit);
	~UIModel();

public:
	void ChangeImage(TextureIndex imageIndex);
	void ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit);

	void SetActiveState(bool state);
	void ToggleActiveState();

	void Update();
	void Render();

	void UpdateScreenTransform();

	UIRect& GetUIRect() { return mUIRect; }

	void AdvanceSprite();
private:
	DirectX::XMFLOAT3X3 Multifly(const DirectX::XMFLOAT3X3& lhs, const DirectX::XMFLOAT3X3& rhs) const;
	DirectX::XMFLOAT3X3 Transpose(const DirectX::XMFLOAT3X3& mat) const;
private:
	std::shared_ptr<I2DRenderable> mUIRenderer{ nullptr };

	bool mActive{ true };
	ModelContext2D mContext{};

	TextureIndex mImageIndex{};
	
#pragma region UISpritable
	bool mSpritable{ false };

	std::pair<UINT, UINT> mImageWidthHeight{};
	std::pair<UINT, UINT> mImageUnit{};

	UINT mSpriteFrameInRow{ 0 };
	UINT mSpriteFrameInCol{ 0 };

	std::pair<UINT, UINT> mSpriteCoord{ 0,0 };
#pragma endregion 

	UIRect mUIRect{};
	DirectX::XMFLOAT3X3 mTransform{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
	DirectX::XMFLOAT3X3 mScreenTransform{};
};


class Canvas {
public:
	Canvas(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<class Window> window);
	~Canvas();
public:
	void Load();

	std::shared_ptr<UIModel> CreateUIModel(TextureIndex imageIndex);
	std::shared_ptr<UIModel> CreateUIModel(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit);

private:
	std::shared_ptr<I2DRenderable> mUIRenderer{ nullptr };

	std::vector<std::shared_ptr<UIModel>> mUIModels{};
};
