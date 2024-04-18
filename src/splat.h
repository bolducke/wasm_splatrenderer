#pragma once

// For Sorting in Parallel
#include <algorithm>
#include <numeric>
#include <execution>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gl.h"

#include "camera.h"
#include "utils.h"


namespace splatwasm {

    struct Splat {
        int count;
        std::vector<glm::vec3> positions{};
        std::vector<float> opacities{};
        std::vector<glm::vec3> colors{};
        std::vector<glm::mat3x3> cov3Ds{};

        Splat();
    };

    void reserve_splat(Splat &splat, int count);

    void order_splat_by_depth(Camera &cam, Splat &data);

    void destroy_splat(Splat &splat);
};