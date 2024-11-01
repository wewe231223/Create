#pragma once 

class Slider {
public:
	Slider(TextureIndex image,POINT LT,UINT width,UINT height);
	~Slider();

private:
	std::unique_ptr<class UIObject> mBase{ nullptr };
	std::unique_ptr<class UIObject> mSlider{ nullptr };
};