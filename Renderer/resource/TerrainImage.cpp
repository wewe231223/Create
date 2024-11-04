#include "pch.h"
#include "resource/TerrainImage.h"
#include "ui/Console.h"

TerrainImage::TerrainImage(const fs::path& path)
{
	std::ifstream file{ path, std::ios::binary | std::ios::ate };
	if (!file.is_open()) {
		Console.WarningLog("{} 파일을 여는데 실패했습니다.", path.string().c_str());
	}

	std::streamsize size = file.tellg();

	// 어지간한 그레이스케일 이미지는 0 ~ 255 사이의 값으로 표현됨
	constexpr int BytesPerPixel = 1;

	int totalBytes = static_cast<int>(size) / BytesPerPixel;

	mWidth = static_cast<int>(std::sqrt(totalBytes));
	mHeight = mWidth;

	if (mWidth * mHeight * BytesPerPixel != totalBytes) {
		Console.WarningLog("{} 파일이 정사각 형식의 파일이 아닙니다. 지형 파일이 올바르게 나타나지 않을 수 있습니다.", path.string().c_str());
	}
	
	file.seekg(0, std::ios::beg);

	std::unique_ptr<BYTE[]> buffer	= std::make_unique<BYTE[]>(sizeof(BYTE) * mWidth * mHeight * BytesPerPixel + 10);
	mPixels							= std::make_unique<BYTE[]>(sizeof(BYTE) * mWidth * mHeight * BytesPerPixel + 10);

	file.read(reinterpret_cast<char*>(buffer.get()), size);
	file.close();

	//// 상하대칭 
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mPixels[x + (y * mWidth)] = buffer[x + ((mHeight - y - 1) * mWidth)];
		}
	}

	Console.InfoLog("{} 파일을 성공적으로 로드했습니다.", path.string().c_str());
}

TerrainImage::TerrainImage(const fs::path& path, int width, int height)
	:mWidth(width), mHeight(height)
{
	std::ifstream file{ path, std::ios::binary };
	if (!file.is_open()) {
		Console.WarningLog("{} 파일을 여는데 실패했습니다.", path.string().c_str());
	}
	// 어지간한 그레이스케일 이미지는 0 ~ 255 사이의 값으로 표현됨
	constexpr int BytesPerPixel = 1;

	std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(sizeof(BYTE) * mWidth * mHeight * BytesPerPixel);
	mPixels = std::make_unique<BYTE[]>(sizeof(BYTE) * mWidth * mHeight * BytesPerPixel);

	file.read(reinterpret_cast<char*>(buffer.get()), mWidth * mHeight * BytesPerPixel);
	file.close();

	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mPixels[x + (y * mWidth)] = buffer[x + ((mHeight - y - 1) * mWidth)];
		}
	}
	Console.InfoLog("{} 파일을 성공적으로 로드했습니다.", path.string().c_str());
}

TerrainImage::~TerrainImage()
{
}

DirectX::SimpleMath::Vector3 TerrainImage::GetNormal(int x, int z, DirectX::SimpleMath::Vector3 scale) const
{
	if (x < 0 || z < 0 || x >= (mWidth - 1) || z >= (mHeight - 1)) {
		return DirectX::SimpleMath::Vector3(0, 1, 0);
	}
	
	// P1(x, z), P2(x+1, z), P3(x, z+1) 점 3개를 이용해서 법선 벡터를 계산한다.
	int index{ x + (z * mWidth) };
	int dx{ x < mWidth - 1 ? 1 : -1 };						// x가 가장 오른쪽 픽셀일 경우 (x-1, z)를 이용
	int dy{ z < mHeight - 1 ? 1 * mWidth : -1 * mWidth };	// z가 가장 아래쪽 픽셀일 경우 (x, z-1)를 이용

	BYTE* p = mPixels.get();
	float y1 = { p[index] * scale.y };			// P1의 y
	float y2 = { p[index + dx] * scale.y };	// P2의 y
	float y3 = { p[index + dy] * scale.y };	// P3의 y

	DirectX::SimpleMath::Vector3 v1{ scale.x, y2 - y1, 0.f }; // P1 -> P2
	DirectX::SimpleMath::Vector3 v2{ 0.f, y3 - y1, scale.z }; // P1 -> P3
	
	DirectX::SimpleMath::Vector3 normal = v2.Cross(v1); // P1P3 x P1P2
	normal.Normalize();

	return normal;
}

float TerrainImage::GetHeight(float x, float z) const
{
	if (x < 0.f || z < 0.f || x >= static_cast<float>(mWidth - 1) || z >= static_cast<float>(mHeight - 1)) {
		return 0.f;
	}

	int ix{ static_cast<int>(x) };	// x의 정수 부분
	int iz{ static_cast<int>(z) };	// z의 정수 부분
	float fx{ x - ix };	// x의 소수 부분
	float fz{ z - iz };	// z의 소수 부분

	BYTE* pixels{ mPixels.get() };
	float LT{ static_cast<float>(pixels[ix + ((iz + 1) * mWidth)]) };		// 좌상단 높이
	float RT{ static_cast<float>(pixels[(ix + 1) + ((iz + 1) * mWidth)]) };	// 우상단 높이
	float LB{ static_cast<float>(pixels[ix + (iz * mWidth)]) };				// 좌하단 높이
	float RB{ static_cast<float>(pixels[(ix + 1) + (iz * mWidth)]) };		// 우하단 높이	

	// 사각형의 네 점을 보간하여 최종 높이 반환
	float topHeight{ Lerp(LT, RT, fx) };						// 보간한 상단 높이
	float botHeight{ Lerp(LB, RB, fx) };						// 보간한 하단 높이
	return Lerp(botHeight,topHeight,fz);						// 보간한 최종 높이
}

int TerrainImage::GetWidth() const
{
	return mWidth;
}

int TerrainImage::GetHeight() const
{
	return mHeight;
}
