#pragma once 
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								UI Model								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								UI Canvas								//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


class Canvas {
public:
	Canvas(ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, std::shared_ptr<class Window> window);
	~Canvas();
public:
	void Load();

	std::shared_ptr<UIModel> CreateUIModel(TextureIndex imageIndex);
	std::shared_ptr<UIModel> CreateUIModel(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit);

	std::shared_ptr<UIModel> CreateUIModel(const std::string& imageName);
	std::shared_ptr<UIModel> CreateUIModel(const std::string& imageName, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit);

	template<typename T, typename... Args> requires std::derived_from<T, IUIObject>
	std::shared_ptr<T> CreateUIObject(Args&&... args);


	// 여기서 각 IUIObject 의 Update 도 호출하고, UIModel 의 Render 도 호출한다. 
	void Update();
	// 여기서는 UIRenderer 의 Render 를 호출해야 한다. 
	void Render(ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	std::shared_ptr<UIRenderer> mUIRenderer{ nullptr };
	// Update 를 위해서
	std::vector<std::shared_ptr<IUIObject>> mUIObjects{};
	// Render 를 위해서
	std::vector<std::shared_ptr<UIModel>> mUIModels{};
};

template<typename T, typename ...Args> requires std::derived_from<T, IUIObject>
inline std::shared_ptr<T> Canvas::CreateUIObject(Args && ...args)
{
	auto res = std::make_shared<T>(this, std::forward<Args>(args)...);
	mUIObjects.emplace_back(res);

	return res;
}

//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								Menu									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class Menu : public IUIObject {
public:
	Menu(Canvas* canvas,const std::string& image, UINT width, UINT height);
	~Menu();
public:
	virtual void Update() override;
private:
	std::shared_ptr<UIModel> mModel{ nullptr };
};





//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//								Slider									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

class Slider : public IUIObject {
public:
	Slider(Canvas* canvas, const std::string& base, const std::string& bar, POINT LT, UINT width, UINT height);
	~Slider();
public:
	virtual void Update() override;
public:
	float mValue{ 0.f };
private:
	std::shared_ptr<UIModel> mBase{ nullptr };
	std::shared_ptr<UIModel> mSlider{ nullptr };

	float mUnit{ 0.f };
};