#include "bar.hpp"
#include "theme/theme.hpp"
#include "avatar.hpp"
#include "imgui.h"
#include "Android_draw/draw.h"
#include <cstdint>
namespace ui::bar {
    void set_game_alpha(float a) { g_game_alpha = a; }
    float game_alpha() { return g_game_alpha; }
    static GLuint avatar_texture = 0;
    static void create_avatar() {
        if (avatar_texture) return;
        glGenTextures(1, &avatar_texture);
        glBindTexture(GL_TEXTURE_2D, avatar_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,HUNTARES_AVATAR_W,HUNTARES_AVATAR_H,
                     0,GL_RGBA,GL_UNSIGNED_BYTE,HUNTARES_AVATAR_RGBA);
    }
    void render() {
        if (g_game_alpha < 0.05f) { g_open=false; return; }
        create_avatar();
        const float r = 35.f;
        const ImVec2 p(24.f, g_sh*0.30f);
        ImGui::SetNextWindowPos(p,ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(r*2+8,r*2+8),ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
        ImGui::Begin("##huntares_avatar",nullptr,
            ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoBackground|
            ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|
            ImGuiWindowFlags_NoScrollbar);
        ImGui::SetCursorPos(ImVec2(4,4));
        if(ImGui::InvisibleButton("##toggle_huntares",ImVec2(r*2,r*2))) g_open=!g_open;
        ImGui::End();
        ImGui::PopStyleVar();
        ImDrawList* draw=ImGui::GetForegroundDrawList();
        ImVec2 a(p.x+4,p.y+4), b(a.x+r*2,a.y+r*2);
        draw->AddCircleFilled(ImVec2(a.x+r,a.y+r),r+3,IM_COL32(0,0,0,190));
        draw->AddImageRounded((ImTextureID)(intptr_t)avatar_texture,a,b,
                              ImVec2(0,0),ImVec2(1,1),IM_COL32_WHITE,r);
        draw->AddCircle(ImVec2(a.x+r,a.y+r),r,IM_COL32(245,245,245,210),48,2.f);
    }
}
