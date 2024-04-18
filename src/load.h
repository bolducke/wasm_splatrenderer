#pragma once
#include <cstring>
#include <string>
#include <iostream>

#include <fstream>
#include <iterator>
#include <algorithm>

#include <cstdio>
#include <ranges>
#include <string_view>

#include <fmt/format.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tinyply.h"

#include "splat.h"

using namespace tinyply;

namespace splatwasm {

    bool load_splat_ply(PathPrefix pp, std::string_view name, Splat &data);
}