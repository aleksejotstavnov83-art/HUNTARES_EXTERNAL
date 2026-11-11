#include "menu.hpp"
#include "bar.hpp"
#include "cfg.hpp"
#include "theme/theme.hpp"
#include "imgui.h"
#include <algorithm>
namespace ui::menu {
    void render() {
        ui::bar::render();
        if (!ui::bar::g_open) return;
        const float w=std::min(405.f,g_sw*.57f);
        const float h=280.f;
        ImGui::SetNextWindowPos(ImVec2(115.f,g_sh*.19f),ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(w,h),ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,14.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(18,14));
        ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.075f,.079f,.087f,.88f));
        ImGui::PushStyleColor(ImGuiCol_Border,ImVec4(.92f,.92f,.92f,.14f));
        ImGui::PushStyleColor(ImGuiCol_CheckMark,ImVec4(1,1,1,1));
        ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(.13f,.13f,.15f,.9f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab,ImVec4(.95f,.95f,.95f,1));
        ImGui::Begin("##huntares_menu",nullptr,ImGuiWindowFlags_NoTitleBar|
             ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoResize|
             ImGuiWindowFlags_NoSavedSettings);
        ImGui::TextColored(ImVec4(1,1,1,1),"HUNTARES EXTERNAL");
        ImGui::TextDisabled("Standoff 2  /  ESP");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0,7));
        ImGui::Checkbox("ESP 2D BOX",&cfg::esp::box);
        ImGui::Dummy(ImVec2(0,7));
        ImGui::SliderFloat("Thickness",&cfg::esp::line_width,1.f,4.f,"%.1f");
        ImGui::Dummy(ImVec2(0,8));
        ImGui::Separator();
        if(ImGui::Button("CLOSE MENU",ImVec2(-1,46.f))) ui::bar::g_open=false;
        ImGui::End();
        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
    }
}
