#pragma once 


inline DirectX::XMFLOAT3X3 Create2DScaleMatrix(float scaleX, float scaleY) {
	return DirectX::XMFLOAT3X3{
		scaleX, 0.0f,   0.0f,
		0.0f,   scaleY, 0.0f,
		0.0f,   0.0f,   1.0f
	};
}

// TODO : 2차원 ViewPort -> NDC 찾아야함. 
inline DirectX::XMFLOAT3X3 CreateScreenTransformMatrix(float screenWidth, float screenHeight) {
	// NDC 좌표를 스크린 좌표로 변환하는 행렬
	return DirectX::XMFLOAT3X3{
		2.f / screenWidth,			0.0f,								0.f,
		0.0f,						-2.f / screenHeight,				0.f,
		2.f / screenWidth,			2.f / screenHeight,					1.f
	};
}

inline DirectX::XMFLOAT3X3 Transpose(const DirectX::XMFLOAT3X3& matrix) {
	return DirectX::XMFLOAT3X3{
		matrix._11, matrix._21, matrix._31,
		matrix._12, matrix._22, matrix._32,
		matrix._13, matrix._23, matrix._33
	};
}