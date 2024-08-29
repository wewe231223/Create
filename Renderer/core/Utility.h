#pragma once 
std::optional<std::wstring> CheckHR(const HRESULT& hr, const std::source_location& location = std::source_location::current() );
