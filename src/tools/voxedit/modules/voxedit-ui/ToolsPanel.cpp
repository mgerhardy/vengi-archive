/**
 * @file
 */

#include "ToolsPanel.h"
#include "IMGUIApp.h"
#include "IconsFontAwesome6.h"
#include "Util.h"
#include "ui/imgui/IMGUIEx.h"
#include "ui/imgui/IconsFontAwesome6.h"
#include "ui/imgui/IconsForkAwesome.h"
#include "voxedit-util/SceneManager.h"

namespace voxedit {

void ToolsPanel::update(const char *title, command::CommandExecutionListener &listener) {
	if (ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoDecoration)) {
		core_trace_scoped(ToolsPanel);

		if (ImGui::CollapsingHeader("Action", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::CommandButton(ICON_FA_CROP, "crop", nullptr, 0, &listener);
			ImGui::SameLine();
			ImGui::CommandButton(ICON_FA_EXPAND, "layersize", nullptr, 0, &listener);
			ImGui::SameLine();
			ImGui::CommandButton(ICON_FA_OBJECT_UNGROUP, "colortolayer", nullptr, 0, &listener);
			ImGui::SameLine();
			ImGui::CommandButton(ICON_FA_COMPRESS, "scale", nullptr, 0, &listener);
			ImGui::SameLine();
			ImGui::CommandButton(ICON_FA_FILL_DRIP, "fillhollow", nullptr, 0, &listener);
		}

		ImGui::NewLine();

		const float buttonWidth = (float)imguiApp()->fontSize() * 4;
		if (ImGui::CollapsingHeader("Rotate on axis", ImGuiTreeNodeFlags_DefaultOpen)) {
			veui::AxisButton(math::Axis::X, "X##rotate", "rotate 90 0 0", ICON_FK_REPEAT, nullptr, buttonWidth,
							 &listener);
			ImGui::TooltipText("Rotate by 90 degree on the x axis");
			ImGui::SameLine();
			veui::AxisButton(math::Axis::Y, "Y##rotate", "rotate 0 90 0", ICON_FK_REPEAT, nullptr, buttonWidth,
							 &listener);
			ImGui::TooltipText("Rotate by 90 degree on the y axis");
			ImGui::SameLine();
			veui::AxisButton(math::Axis::Z, "Z##rotate", "rotate 0 0 90", ICON_FK_REPEAT, nullptr, buttonWidth,
							 &listener);
			ImGui::TooltipText("Rotate by 90 degree on the z axis");
		}

		ImGui::NewLine();

		if (ImGui::CollapsingHeader("Flip on axis", ImGuiTreeNodeFlags_DefaultOpen)) {
			veui::AxisButton(math::Axis::X, "X##flip", "flip x", nullptr, nullptr, buttonWidth, &listener);
			ImGui::SameLine();
			veui::AxisButton(math::Axis::Y, "Y##flip", "flip y", nullptr, nullptr, buttonWidth, &listener);
			ImGui::SameLine();
			veui::AxisButton(math::Axis::Z, "Z##flip", "flip z", nullptr, nullptr, buttonWidth, &listener);
		}
	}
	ImGui::End();
}

} // namespace voxedit
