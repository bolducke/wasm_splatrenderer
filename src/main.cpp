#include "app.h"

#include <fmt/format.h>

#include "load.h"


using namespace splatwasm;

int main(int argc, char** argv) {

    App app;

    if (!create_app(app)) {
        return 1;
    }

    Splat splat;
    if (!load_splat_ply(app.pp, "lego_bouquet.ply", splat)) {
        fmt::println("Failed loading .ply file");
        return 1;;
    }

    if(!load_scene_splat(app, splat)) {
        fmt::println("Failed loading scene");
        return 1;
    }

    run(app);

    destroy_app(app);

    return 0;
}