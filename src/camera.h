#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

namespace splatwasm {

    enum MovementDirection {
        FORWARD,
        BACKWARD,
        UPWARD,
        DOWNWARD,
        LEFTWARD,
        RIGHTWARD
    };

    // Very simple camera inspired of one in OpenGL Learn.
    struct Camera {
        glm::mat4 proj;
        glm::mat4 view;

        glm::vec3 position;

        glm::vec3 up;
        glm::vec3 front;
        glm::vec3 right;

        glm::vec3 world_up;

        float yaw;
        float pitch;

        float acc;
        float mouse_sensitivity;
    };

    Camera create_camera(glm::mat4 proj, glm::vec3 position = glm::vec3(0.f,0.f,0.f), glm::vec3 world_up =  glm::vec3(0.f,1.f,0.f), float yaw = 90.f, float pitch = 0.f);

    void handle_camera_rotation_mouse_motion(Camera& cam, float dx, float dy);

    void handle_camera_movement_keyboard_event(Camera& cam, MovementDirection dir, float dt);

    void sync_camera_view(Camera &camera);
    void sync_camera_yaw_pitch(Camera &cam);
}