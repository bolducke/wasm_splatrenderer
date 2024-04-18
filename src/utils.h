#pragma once

#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>

#include <cstdio>
#include <ranges>
#include <string_view>

#include <fmt/format.h>
#include "gl.h"

namespace splatwasm {

    struct PathPrefix {
        std::string_view shader;
        std::string_view scene;
    };

    bool load_shader(PathPrefix pp, std::string_view filename, std::string &source);

    bool create_and_compile_shader(std::string_view source, GLenum mode, GLuint &shader);

    GLuint create_and_link_program(GLuint* shaders, int count);
}