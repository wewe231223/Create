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

	ImGui::EndChild();


	static char buf[256] = "";
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	if (ImGui::InputText("##ChatInput", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (strlen(buf) > 0) {
			
			ChatWindow::UpdateChatLog("{:^10} : {}","Me",buf);

			ZeroMemory(buf, sizeof(buf));
			ImGui::SetKeyboardFocusHere(-1); // 입력 창에 포커스 유지

		}
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::End();
}
