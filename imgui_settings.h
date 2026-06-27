#include "imgui.h"

namespace c
{
	// === Black / neutral theme ===
	inline ImVec4 accent = ImColor(210, 210, 215);
	inline ImVec4 similarColor = ImColor(75, 75, 80);

	inline ImVec4 green = ImColor(0, 255, 0);
	inline ImVec4 orange = ImColor(160, 160, 168);
	inline ImVec4 red = ImColor(220, 70, 70);

	// === Background ===
	namespace background
	{
		inline ImVec4 filling = ImColor(8, 8, 10, 235);
		inline ImVec4 stroke = ImColor(28, 28, 32, 180);
		inline ImVec2 size = ImVec2(600, 350);

		inline float rounding = 8.f;
	}

	// === Elements ===
	namespace element
	{
		inline ImVec4 filling = ImColor(18, 18, 20, 210);
		inline ImVec4 stroke = ImColor(45, 45, 50, 150);
		inline ImVec4 panel = ImColor(12, 12, 14, 160);

		inline float rounding = 8.f;
	}

	// === Text ===
	namespace text
	{
		inline ImVec4 text_active = ImColor(245, 245, 248);
		inline ImVec4 text_hov = ImColor(200, 200, 208);
		inline ImVec4 text = ImColor(170, 170, 178);
	}
}
