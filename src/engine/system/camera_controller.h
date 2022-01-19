#ifndef CAMERA_CONTROLLER_H_
#define CAMERA_CONTROLLER_H_

#include <numbers>

#include "../component/camera.h"
#include "../data/camera_event.h"
#include "../utils/mat_opengl.h"
#include "../utils/math.h"
#include "engine/utils/mat.h"
#include "event_manager.h"

class CameraController {
   public:
    void subscribeOnCameraMoveEvent() {
        EventManager::subscribe(
            std::function{[&](const CameraMoveEvent& event) {
                this->onCameraMoveEvent(event);
            }});
    }
    void onCameraMoveEvent(const CameraMoveEvent& event) {
        camera.self.speed = camera.self.base_speed * event.translationAxis;
        camera.self.rotation_speed =
            camera.self.base_rotation_speed * event.rotationAxis;
    }

    void update(float dt) {
        // Lock rotation if rotation goes too far
        constexpr double sin_yz_lim =
            math::sin(std::numbers::pi / 10);  // =  9deg
        const float sin_yz_2 =
            math::norm2(math::wedge(math::get_view3(camera.self.rotation[1]),
                                    math::get_view3(camera.self.rotation[2])));

        if (sin_yz_2 < sin_yz_lim * sin_yz_lim) {
            float cos_yz =
                math::dot(camera.self.rotation[1], camera.self.rotation[2]);

            // looking at sky or at ground, dont keep going
            if (cos_yz * camera.self.rotation_speed[0] < 0)
                camera.self.rotation_speed[0] = 0;
        }
        camera.self.update(dt);
    }

    void updateCameraViewport(int width, int height) {
        camera.persp =
            math::projection((float)width / (float)height, fov, near, far);
    }

    [[nodiscard]] constexpr math::vec3f get_position(
        bool absolute = true) const {
        if (absolute && camera.parent)
            return camera.parent->position + camera.self.position;
        return camera.self.position;
    }

    [[nodiscard]] constexpr math::mat4f get_rotation(
        bool absolute = true) const {
        if (absolute && camera.parent)
            return camera.self.rotation * camera.parent->rotation;
        return camera.self.rotation;
    }

    [[nodiscard]] constexpr math::mat4f getCameraMatrix() const {
        // We move world by -transl, this the same effect as moving camera by
        // transl;
        auto position = get_position();
        auto rotation = get_rotation();
        auto transl =
            math::translation(-position[0], -position[1], -position[2]);
        return camera.persp * rotation * transl;
    }

    Camera camera{};

   private:
    const float fov = std::numbers::pi_v<float> / 2;
    const float near = 1.;
    const float far = 80.;

    Logger log{Logger::get({"CameraController"})};
};

#endif  // !CAMERA_CONTROLLER_H_
