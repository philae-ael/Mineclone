#ifndef CAMERA_CONTROLLER_H_
#define CAMERA_CONTROLLER_H_

#include <cmath>

#include "../component/camera.h"
#include "../component/camera_event.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"
#include "event_manager.h"

class CameraController {
   public:
    CameraController() {
        EventManager::subscribe(
            std::function{[&](const CameraMoveEvent& event) {
                this->onCameraMoveEvent(event);
            }});
    };

    void onCameraMoveEvent(const CameraMoveEvent& event) {
        Logger::get() << "In onCameraMoveEvent";

        translation_movement_axis = math::normalize(event.translationAxis);
    }

    void update(float dt) {
        const math::vec3f direction_x =
            math::normalize(math::get_view<float, 4, 3>(camera.rotation[0]));
        const math::vec3f direction_y =
            math::normalize(math::get_view<float, 4, 3>(camera.rotation[1]));
        const math::vec3f direction_z =
            math::normalize(math::get_view<float, 4, 3>(camera.rotation[2]));

        const math::vec3f direction =
            (translation_movement_axis[0] * direction_x +
             translation_movement_axis[1] * direction_y +
             translation_movement_axis[2] * direction_z);

        camera.position += dt * speed * direction;

        Logger::get() << direction;
    }

    void updateCamera(int width, int height) {
        camera.persp =
            math::projection((float)width / (float)height, fov, near, far);
    }

    Camera& getCamera() { return camera; }

    // NOTE: This should be called AFTER position is set
    constexpr void lookAt(math::vec3f point) {
        auto v = math::normalize(point - camera.position);

        // the aim is to enode a rotation that maps (0, 0, -1) onto
        // v; Or (0,0,1) onto -v.
        // We can achieve this by composing only two rotation, one along the x
        // axis and one along the y axix
        const math::vec3f x{1, 0, 0}, y{0, 1, 0}, z{0, 0, 1};
        v = -v;  // now we map (0, 0, 1) onto v

        // y rot
        auto v_proj_y = math::normalize(v - math::dot(v, y) * y);
        if (math::norm2(v_proj_y) == 0) v_proj_y = z;
        const auto v_perp_y = math::normalize(math::wedge(y, v_proj_y));

        const math::mat3f rot_y{{
            {v_perp_y[0], 0, v_perp_y[2]},
            {0, 1, 0},
            {v_proj_y[0], 0, v_proj_y[2]},
        }};

        // Change v of base
        const auto v2 = rot_y * v;
        // v2 should be on the (Oyz) plane, no need to project

        // x rot
        auto v_proj_x = math::normalize(v2);
        if (math::norm2(v_proj_x) == 0) v_proj_x = z;
        const auto v_perp_x = math::normalize(math::wedge(v_proj_x, x));

        const math::mat3f rot_x{{
            {1, 0, 0},
            {0, v_perp_x[1], v_perp_x[2]},
            {0, v_proj_x[1], v_proj_x[2]},
        }};

        camera.rotation = math::extend<float, 3, 3, 4>(rot_x * rot_y);
    }

    [[nodiscard]] constexpr math::mat4f getCameraMatrix() const {
        // We move world by -transl, this the same effect as moving camera by
        // transl;
        auto transl = math::translation(
            -camera.position[0], -camera.position[1], -camera.position[2]);
        return camera.persp * camera.rotation * transl;
    }

   private:
    Camera camera{};
    const float fov = M_PI / 2;
    const float near = 1.;
    const float far = 40.;

    const float speed = 5;
    math::vec3f translation_movement_axis{};
};

#endif  // !CAMERA_CONTROLLER_H_
