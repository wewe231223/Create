#pragma once 

enum EComponentType : UINT {
	Transform,
	END,
	NONE,
};

class Component {
public:
	const static EComponentType Type = EComponentType::NONE;
	virtual ~Component() = default;
};
