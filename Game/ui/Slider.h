#pragma once 

class Slider {
public:
	Slider(std::shared_ptr<I2DRenderable> uiRenderer,TextureIndex image,POINT LT,UINT width,UINT height);
	~Slider();

private:
	std::unique_ptr<class UIModel> mBase{ nullptr };
	std::unique_ptr<class UIModel> mSlider{ nullptr };

	float mUnit{ 0.f };
	float mValue{ 0.f };
};