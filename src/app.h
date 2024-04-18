#pragma once

#include <fmt/format.h>
#include <iostream>

#include "SDL2/SDL.h"

#include "gl.h"

#include "app.fwd.h"
#include "model.fwd.h"

#include "splat.h"
#include "camera.h"
#include "load.h"
#include "model.h"

namespace splatwasm {

    bool create_app(App& app);

    bool load_scene_splat(App& app, Splat data);

    void destroy_app(App& app);

    void run(App& app);
}