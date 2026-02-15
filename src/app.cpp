#include "app.h"

namespace splatwasm {

    bool create_app(App& app) {
        app.is_running = false;
        app.is_mouse_pressed = false;

        app.W = 680; app.H = 680;

        PathPrefix pp {.shader="data/shader/", .scene="data/splat/"};
        app.pp = pp;

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        app.wnd = SDL_CreateWindow("Splat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    app.W, app.H, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        SDL_GL_SetSwapInterval(0);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

        app.glc = SDL_GL_CreateContext(app.wnd);

        #if __EMSCRIPTEN__
            EmscriptenWebGLContextAttributes attr{};
            emscripten_webgl_init_context_attributes(&attr);

            attr.majorVersion = 2;
            attr.minorVersion = 0;
            attr.alpha = true;
            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE wgl_ctx = emscripten_webgl_create_context("#canvas", &attr);
            emscripten_webgl_make_context_current(wgl_ctx);
        #else
            if (!gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress)) {
                return false;
            }
        #endif

        return true;
    }

    bool load_scene_splat(App& app, Splat data) {
        // Create Camera
        app.cam = create_camera(glm::perspective(45.f, float(app.W) / float(app.H), .1f, 50.f), glm::vec3(0.f,0.f,1.f), glm::vec3(0.,-1.,0.));

        if (!create_model(app.pp, data, app.model)) {
            return false;
        }

        order_splat_by_depth(app.cam, app.model.splat);
        update_buffers(app.model);

        return true;
    }

    void destroy_app(App& app) {
        SDL_GL_DeleteContext(app.glc);
        SDL_DestroyWindow(app.wnd);
        SDL_Quit();
    }

    void _run(void *arg) {
        App *app = (App*)arg; // Cast to App* and then dereference to app

        app->time_curr_frame = SDL_GetTicks();

        glClearColor(0.f,0.f,0.f,0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Disable Depth Test and Enable Blend
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

        float dt = float(app->time_curr_frame - app->time_last_frame)/1000.f;

        SDL_Event e;
        bool need_update = false;
        bool is_mouse_pressed = true;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) {
               app->is_running = false;
            }

            switch (e.type) {

                // Very simple way to handle input.
                // TODO: Should handle each input as an "action" with a keymap instead.
                case SDL_MOUSEBUTTONDOWN:
                {
                    app->is_mouse_pressed = true;

                    SDL_GetMouseState(&app->mouse_x, &app->mouse_y);
                    app->last_mouse_y = app->mouse_y;
                    app->last_mouse_x = app->mouse_x;
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    app->is_mouse_pressed = false;
                    app->last_mouse_y = app->mouse_y;
                    app->last_mouse_x = app->mouse_x;
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if (app->is_mouse_pressed) {
                        SDL_GetMouseState(&app->mouse_x, &app->mouse_y);

                        auto mouse_dx = static_cast<float>(app->mouse_x - app->last_mouse_x);
                        auto mouse_dy = -static_cast<float>(app->mouse_y - app->last_mouse_y);

                        handle_camera_rotation_mouse_motion(app->cam, mouse_dx, mouse_dy);
                        need_update = true;
                    }

                    app->last_mouse_y = app->mouse_y;
                    app->last_mouse_x = app->mouse_x;

                    break;
                }
                case SDL_KEYDOWN:
                {
                    switch (e.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        {
                            handle_camera_movement_keyboard_event(app->cam, MovementDirection::FORWARD, dt);
                            need_update = false;
                            break;
                        }
                        case SDL_SCANCODE_S:
                        {
                            handle_camera_movement_keyboard_event(app->cam, MovementDirection::BACKWARD, dt);
                            need_update = false;
                            break;
                        }
                        case SDL_SCANCODE_D:
                        {
                            handle_camera_movement_keyboard_event(app->cam, MovementDirection::RIGHTWARD, dt);
                            need_update = false;
                            break;
                        }
                        case SDL_SCANCODE_A:
                        {
                            handle_camera_movement_keyboard_event(app->cam, MovementDirection::LEFTWARD, dt);
                            need_update = false;
                            break;
                        }
                    }

                    if (e.key.keysym.mod & KMOD_CTRL) {
                        handle_camera_movement_keyboard_event(app->cam, MovementDirection::DOWNWARD, dt);
                        need_update = false;
                    } else if (e.key.keysym.mod & KMOD_SHIFT) {
                        handle_camera_movement_keyboard_event(app->cam, MovementDirection::UPWARD, dt);
                        need_update = false;
                    }
                    break;
                }
            }
        }

        if (need_update) {
            order_splat_by_depth(app->cam, app->model.splat);
            update_buffers(app->model);
        }

        render(*app, app->model);

        SDL_GL_SwapWindow(app->wnd);
        app->time_last_frame = app->time_curr_frame;
    }

    void run(App &app) {
        app.is_running = true;
        #ifdef __EMSCRIPTEN__
            emscripten_set_main_loop_arg(&_run, &app, 0, 1);
        #else
            while(app.is_running) {
                _run(&app);
            }
        #endif
    }

}