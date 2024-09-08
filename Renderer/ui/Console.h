#pragma once 

class GConsole {
public:
	GConsole();
	~GConsole();

	void Render();

	template<typename... Args>
	void InfoLog(std::format_string<Args...> fmt, Args&&... args)
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm local_time{};
		localtime_s(&local_time, &now_time);



		mLog.Emplace(
			std::make_pair(std::move(std::format("[ INFO - {:02}:{:02}:{:02} ] ", local_time.tm_hour, local_time.tm_min, local_time.tm_sec)), F4_YELLOW_GREEN),
			std::format(fmt, std::forward<Args>(args)...)
		);
	}

	template<typename... Args>
	void WarningLog(std::format_string<Args...> fmt, Args&&... args)
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm local_time{};
		localtime_s(&local_time, &now_time);



		mLog.Emplace(
			std::make_pair(std::move(std::format("[ WARN - {:02}:{:02}:{:02} ] ", local_time.tm_hour, local_time.tm_min, local_time.tm_sec)), F4_WHEAT),
			std::format(fmt, std::forward<Args>(args)...)
		);
	}

private:
	static constexpr const char* mTitle = "Console";
	RingBuffer<std::pair<std::pair<std::string, ImVec4>, std::string>, 100>	mLog{};
};

extern GConsole Console;