
// ================================================================================
// Package distributed via @imguisource_bot
// Channel ImGui Source: @ImGui_Source_FIXNEY
// Channel shit src 🧨: @il2cpp_src
// ================================================================================

#include "Android_draw/draw.h"
#include "ui/theme/theme.hpp"
#include "ui/menu.hpp"
#include "ui/bar.hpp"
#include "other/memory.hpp"
#include "game/game.hpp"
#include "func/visuals.hpp"
#include "protect/oxorany.hpp"
#include <cstdio>
#include <thread>
#include <chrono>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
static void print_status(const char* status) {
    diag::log("STATUS", "%s", status);
}

// Функция для запуска Standoff
static void launch_standoff() {
    int rc = system(oxorany("monkey -p com.axlebolt.standoff2 -c android.intent.category.LAUNCHER 1"));
    diag::log(rc == 0 ? "INFO" : "WARN", "game launch command exit=%d package=com.axlebolt.standoff2", rc);
}

static bool is_game_foreground() {
    // Only draw while the activity that owns the foreground is Standoff 2.
    // A strict fail-closed policy means no floating menu on the home screen.
    FILE* pipe = popen("dumpsys activity activities 2>/dev/null | grep -m 1 -E 'topResumedActivity|mResumedActivity|ResumedActivity'", "r");
    if (!pipe) return false;
    char line[768]{};
    bool ok = fgets(line, sizeof(line), pipe) != nullptr;
    pclose(pipe);
    if (ok) return strstr(line, "com.axlebolt.standoff2") != nullptr;
    pipe = popen("dumpsys window 2>/dev/null | grep -m 1 -E 'mCurrentFocus|mFocusedApp'", "r");
    if (!pipe) return false;
    ok = fgets(line, sizeof(line), pipe) != nullptr;
    pclose(pipe);
    return ok && strstr(line, "com.axlebolt.standoff2") != nullptr;
}

int main() {
    const int lock = open("/data/local/tmp/huntares-external.lock",O_CREAT|O_RDWR,0600);
    if (lock < 0 || flock(lock,LOCK_EX|LOCK_NB) != 0) return 2;
    diag::log("INFO","HUNTARES External launching native ESP engine");
    screen_config();
    int max_size = displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width;
    int min_size = displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width;
    g_sw=(float)max_size;
    g_sh=(float)min_size;
    native_window_screen_x=max_size;
    native_window_screen_y=max_size;
    if (!initGUI_draw(max_size,max_size,true)) return 3;
    touch::init(max_size,min_size,(uint8_t)displayInfo.orientation);
    launch_standoff();
    game::init();
    bool game_seen=false;
    int missing_ticks=0;
    long long last_process_check=0;
    long long last_focus=0;
    bool in_focus=false;
    while (true) {
        const bool run=game::valid();
        if (diag::every(last_process_check,1000)) {
            int current_pid=get_pid();
            if (current_pid > 0) { game_seen=true; missing_ticks=0; }
            else if (game_seen && ++missing_ticks >= 4) break;
            if (proc::pid > 0 && current_pid < 0) {
                proc::pid=-1;
                proc::lib=0;
            }
        }
        if (diag::every(last_focus,650)) in_focus=is_game_foreground();
        const bool is_landscape=(displayInfo.orientation==1 || displayInfo.orientation==3);
        const bool draw_ui=proc::pid > 0 && in_focus && is_landscape;
        drawBegin();
        ui::bar::set_game_alpha(draw_ui ? 1.f : 0.f);
        if (draw_ui) {
            ui::menu::render();
            if (run && proc::lib) { game::check_lib(proc::lib); visuals::draw(); }
        } else ui::bar::g_open=false;
        drawEnd();
        usleep(draw_ui?6000:60000);
    }
    shutdown();
    return 0;
}
