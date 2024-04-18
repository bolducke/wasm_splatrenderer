//
// Created by lodrik on 24-04-12.
//
#include <vector>

#include <glm/glm.hpp>

#include "splat.h"

namespace splatwasm {
    // Adapted from https://gist.github.com/HViktorTsoi/58eabb4f7c5a303ced400bcfa816f6f5
    std::vector<int> argsort_z(const std::vector<glm::vec3> &array) {
        std::vector<int> indices(array.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(std::execution::par_unseq, indices.begin(), indices.end(),
                         [&array](int left, int right) -> bool {
                             return array[left].z > array[right].z;
                         });

        return indices;
    }

    Splat::Splat() {
        count = 0;
    }

    void reserve_splat(Splat &splat, int count) {
        splat.count = count;
        splat.positions.reserve(count);
        splat.colors.reserve(count);
        splat.opacities.reserve(count);
        splat.cov3Ds.reserve(count);
    }

    void order_splat_by_depth(Camera &cam, Splat &splat) {

        std::vector<glm::vec3> view_position; view_position.reserve(splat.count);
        for (int i = 0; i < splat.count; i += 1) {
            view_position.emplace_back(cam.view * glm::vec4(splat.positions[i],1.));
        }
        auto ordered_indices = argsort_z(view_position); // Order by their z value (depth)

        Splat ordered{};
        reserve_splat(ordered, splat.count);

        for(int idx = 0; idx < ordered.count; idx += 1) {
            int ordered_idx = ordered_indices[idx];

            ordered.positions.emplace_back(splat.positions.at(ordered_idx));
            ordered.colors.emplace_back(splat.colors.at(ordered_idx));
            ordered.cov3Ds.emplace_back(splat.cov3Ds.at(ordered_idx));
            ordered.opacities.emplace_back(splat.opacities.at(ordered_idx));
        }

        splat = std::move(ordered); // Avoid copying vector.
    }

    void destroy_splat(Splat &splat) {
        splat.count = -1;

        // Clear each array of their value.
        splat.positions.clear();
        splat.opacities.clear();
        splat.colors.clear();
        splat.cov3Ds.clear();
    }
}