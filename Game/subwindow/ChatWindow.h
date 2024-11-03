#pragma once 

class ChatWindow {
public:
	ChatWindow();
	~ChatWindow();

public:
	template<typename ...Args> 
	void UpdateChatLog(std::format_string<Args...> fmt, Args&&... args);
	void Render();
private:
	std::vector<std::string> mChatLog{};
	std::vector<std::string> mInputChat{};
};

/*
	template<typename... Args>
	void InfoLog(std::format_string<Args...> fmt, Args&&... args)

*/

template<typename ...Args>
inline void ChatWindow::UpdateChatLog(std::format_string<Args...> fmt, Args && ...args)
{
	mChatLog.emplace_back(std::format(fmt, std::forward<Args>(args)...));
}
