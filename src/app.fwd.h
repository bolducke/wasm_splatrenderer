#pragma once

#include "SDL2/SDL.h"

#include "gl.h"

#include "utils.h"
#include "camera.h"
#include "model.fwd.h"

namespace splatwasm {
    // Only one render target where the content is rendered fullscreen.
    struct App {
        // Handle & more
        SDL_Window* wnd{};
        SDL_GLContext glc{};
        PathPrefix pp{}; // Zero-Initialize

        // App State
        bool is_running{};
        int W{}, H{};

        bool is_mouse_pressed;

        int last_mouse_x, last_mouse_y;
        int mouse_x, mouse_y;
        bool first_mouse_motion;

        uint32_t time_last_frame, time_curr_frame;

        // Scene - Constructed at loading time.
        Camera cam{};  // Zero-Initialize
        Model model{}; // Zero-Initialize
    };
}