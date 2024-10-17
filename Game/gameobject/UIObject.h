#pragma once 

class UIObject {
public:
	struct UIRect {
		float LTx = 0.f;
		float LTy = 0.f;
		float width = 1.f;
		float height = 1.f;
	};
public:
	UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex);
	UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, const std::pair<UINT, UINT>& imageUnit);
	~UIObject();

public:
	void ChangeImage(TextureIndex imageIndex);
	void ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight,const std::pair<UINT, UINT>& imageUnit);


	void Disable();
	void Enable();

	void Update();
	void Render();

	void UpdateScreenTransform();
private:
	DirectX::XMFLOAT3X3 Multifly(const DirectX::XMFLOAT3X3& lhs, const DirectX::XMFLOAT3X3& rhs) const;
	DirectX::XMFLOAT3X3 Transpose(const DirectX::XMFLOAT3X3& mat) const;
private:
	std::shared_ptr<I2DRenderable> mUIRenderer{ nullptr };

	ModelContext2D mContext{};

	TextureIndex mImageIndex{};
	
	std::pair<UINT, UINT> mImageWidthHeight{};
	std::pair<UINT, UINT> mImageUnit{};

	UIRect mUIRect{};
	DirectX::XMFLOAT3X3 mTransform{};

	bool mActive{ true };
	bool mSpritable{ false };
};


