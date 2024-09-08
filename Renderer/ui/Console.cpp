#include "pch.h"
#include "ui/Console.h"

constexpr size_t ConsoleLogReserveSize = 100;
GConsole::GConsole()
{

}

GConsole::~GConsole()
{
}

void GConsole::Render()
{
	ImGui::Begin(mTitle);

	for (const auto& msg : mLog){
		const auto& [head, color] = msg.first;

		ImGui::TextColored(color, head.c_str());
		ImGui::SameLine(0.f, 10.f);
		ImGui::TextWrapped(msg.second.c_str());
	}
	

	ImGui::SetScrollHereY(1.0f);

	ImGui::End();
}

GConsole Console{};


