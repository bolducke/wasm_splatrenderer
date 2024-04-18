#pragma once

#include "gl.h"

#include <fmt/format.h>

#include "model.fwd.h"
#include "app.fwd.h"

#include "splat.h"
#include "camera.h"

namespace splatwasm {

// Could be a better name...
    bool create_model(PathPrefix pp, Splat splat, Model &model);

    void update_buffers(Model &model);
    void render(App &app, Model &model);

    void destroy_model(Model &model);
}