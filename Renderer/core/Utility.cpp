#include "pch.h"
#include "core/Utility.h"


constexpr unsigned int ErrorMessageBoxOpt = 0x00000010L | 0x00000000L;

std::wstring to_wstring(const std::string& str)
{
	return std::wstring{ str.begin(), str.end() };
}


std::optional<std::wstring> CheckHR(const HRESULT& hr, const std::source_location& location)
{
	std::optional<std::wstring> result{ std::nullopt };
	if (FAILED(hr))
	{
		_com_error err{ hr };
		result = std::format(L"오류 위치 : {}\n{} \n\n Error : \n{}", to_wstring(location.file_name()), to_wstring(location.function_name()), err.ErrorMessage());
		::MessageBoxW(nullptr, result.value().c_str(), L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}
	return result;
}
