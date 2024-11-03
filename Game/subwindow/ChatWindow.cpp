#include "pch.h"
#include "Game/subwindow/ChatWindow.h"

ChatWindow::ChatWindow()
{
}

ChatWindow::~ChatWindow()
{
}


void ChatWindow::Render()
{
	ImGui::Begin("ChatWindow");

	ImGui::BeginChild("ChatLog", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);

	for (const auto& msg : mChatLog) {
		ImGui::TextWrapped(msg.c_str());
	}

	ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();


	static char buf[100] = "";
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	if (ImGui::InputText("##ChatInput", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
		size_t len = strlen(buf);
		if (len > 0 and len < 98) {
			mInputChat.emplace_back(buf);
			ZeroMemory(buf, sizeof(buf));
			ImGui::SetKeyboardFocusHere(-1); // 입력 창에 포커스 유지
		}
	}

	ImGui::End();
}
