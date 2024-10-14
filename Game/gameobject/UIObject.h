#pragma once 

// 대강 틀만 잡았다. 여기서는 구현하기 쉬울것. 로딩 스프라이트 렌더링부터 만들어라. 
class UIObject {
public:
	UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex);
	UIObject(std::shared_ptr<I2DRenderable> uiRenderer, TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, std::pair<UINT, UINT> mImageUnit);
	~UIObject();

public:
	void ChangeImage(TextureIndex imageIndex);
	void ChangeImage(TextureIndex imageIndex, const std::pair<UINT, UINT>& imageWidthHeight, std::pair<UINT, UINT> mImageUnit);

	void Update();
	void Render();
private:
	std::shared_ptr<I2DRenderable> mUIRenderer;
	TextureIndex mImageIndex;
	std::pair<UINT, UINT> mImageWidthHeight;
	std::pair<UINT, UINT> mImageUnit;

	bool mSpritable{ false };
};