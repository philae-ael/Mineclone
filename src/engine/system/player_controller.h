#ifndef PLAYER_CONTROLLER_H_
#define PLAYER_CONTROLLER_H_

#include "../component/player.h"
#include "camera_controller.h"
#include "engine/utils/mat.h"

class PlayerController {
   public:
    PlayerController(CameraController* controller) {
        attach_camera(controller);

        EventManager::subscribe(std::function{
            [&](const CameraMoveEvent& event) { this->onPlayerMove(event); }});
    }

    void onPlayerMove(const CameraMoveEvent& event) {
        // Player can only rotate around the Y axis
        player.self.speed = player.self.base_speed * event.translationAxis;
        player.self.rotation_speed[1] =
            player.self.base_rotation_speed * event.rotationAxis[1];

        // Dont process twice the event
        auto copy = event;
        copy.rotationAxis[1] = {};
        copy.translationAxis = {};

        if (camera_controller) {
            camera_controller->onCameraMoveEvent(copy);
        }
    }

    void update(float dt) { 
        player.self.update(dt); 
    }

    void attach_camera(CameraController* controller) {
        camera_controller = controller;
        camera_controller->camera.parent = &player.self;
        camera_controller->camera.self = {};
    }
    void detach_camera() {
        camera_controller->camera.parent = nullptr;
        camera_controller = nullptr;
    }


    Player player{};
   private:
    CameraController* camera_controller = nullptr;
};

#endif  // PLAYER_CONTROLLER_H_
