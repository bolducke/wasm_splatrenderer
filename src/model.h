#pragma once

#include "gl.h"

#include <fmt/format.h>

#include "splat.h"
#include "camera.h"
#include "utils.h"

namespace splatwasm {

    struct App;

    struct Model {
        Splat splat;

        // Material & Attribute to bind - Do not need anything fancy for our use case.
        GLuint program;
        GLuint vao;

        // Buffer Related Information to the splat;
        GLuint vbo_position;
        GLuint vbo_color;
        GLuint vbo_opacity;
        GLuint vbo_cov3d;
    };

    // Could be a better name...
    bool create_model(PathPrefix pp, Splat splat, Model &model);

    void update_buffers(Model &model);
    void render(App &app, Model &model);

    void destroy_model(Model &model);
}