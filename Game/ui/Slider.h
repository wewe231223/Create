#pragma once 

class Slider : public IUIObject {
public:
	Slider(class Canvas* canvas, const std::string& base, const std::string& bar, POINT LT, UINT width, UINT height);
	~Slider();
public:
	virtual void Update() override;
public:
	float mValue{ 0.f };
private:
	std::shared_ptr<class UIModel> mBase{ nullptr };
	std::shared_ptr<class UIModel> mSlider{ nullptr };

	float mUnit{ 0.f };
};