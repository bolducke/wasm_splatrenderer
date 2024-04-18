#include "camera.h"

namespace splatwasm {

    void sync_camera_view(Camera &cam) {
        cam.view = glm::lookAt(cam.position, cam.position + cam.front, cam.up);
    }

    void sync_camera_yaw_pitch(Camera &cam) {
        glm::vec3 front;
        front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
        front.y = sin(glm::radians(cam.pitch));
        front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));

        cam.front = glm::normalize(front);
        cam.right = glm::normalize(glm::cross(cam.front, cam.world_up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        cam.up    = glm::normalize(glm::cross(cam.right, cam.front));

        sync_camera_view(cam);
    }

    Camera create_camera(glm::mat4 proj, glm::vec3 position, glm::vec3 world_up, float yaw, float pitch) {
        Camera cam{
            .proj = proj,

            .position = position,

            .world_up = world_up,

            .yaw = yaw,
            .pitch = pitch,

            .acc = 1.f,
            .mouse_sensitivity = .07f,
        };

        sync_camera_yaw_pitch(cam);

        return cam;
    }

    void handle_camera_rotation_mouse_motion(Camera &cam, float dx, float dy) {
        dx *= cam.mouse_sensitivity;
        dy *= cam.mouse_sensitivity;

        cam.yaw   += dx;
        cam.pitch += dy;

        if (cam.pitch > 89.0f)
            cam.pitch = 89.0f;
        if (cam.pitch < -89.0f)
            cam.pitch = -89.0f;

        sync_camera_yaw_pitch(cam);
    }

    void handle_camera_movement_keyboard_event(Camera &cam, MovementDirection dir, float dt) {
        float cam_speed = cam.acc * dt;

        switch (dir) {
            case MovementDirection::FORWARD:{
                cam.position += cam.front * cam_speed;
                break;
            }
            case MovementDirection::BACKWARD: {
                cam.position -= cam.front * cam_speed;
                break;
            }
            case MovementDirection::UPWARD: {
                cam.position += cam.up * cam_speed;
                break;
            }
            case MovementDirection::DOWNWARD: {
                cam.position -= cam.up * cam_speed;
                break;
            }
            case MovementDirection::RIGHTWARD: {
                cam.position += cam.right * cam_speed;
                break;
            }
            case MovementDirection::LEFTWARD: {
                cam.position -= cam.right * cam_speed;
                break;
            }
        }

        sync_camera_view(cam);
    }
}