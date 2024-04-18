// no #pragma once because we want to include it in every header.

#ifdef __EMSCRIPTEN__
#include <GLES3/gl32.h>
#include "emscripten/emscripten.h"
#include "emscripten/html5.h"
#else
#include "glad/glad.h"
#endif