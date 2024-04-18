#pragma once

#include "gl.h"

#include "splat.h"

namespace splatwasm {
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
}