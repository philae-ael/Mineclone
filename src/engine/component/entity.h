#ifndef ENTITY_H_
#define ENTITY_H_

#include <numbers>

#include "engine/utils/logging.h"
#include "engine/utils/mat.h"

struct Entity {
    math::vec3f position{};
    math::vec3f speed{};

    math::mat4f rotation = math::identity<float, 4>();
    math::vec3f rotation_speed{};

    float base_speed = 10;
    float base_rotation_speed = std::numbers::pi;

    void update(float dt) {
        // Y axis is always set to 0 1 0
        // Z axis is the direction which the object is facing
        const math::mat3f direction_raw = {
            math::get_view3(rotation[0]),
            math::get_view3(rotation[1]),
            math::get_view3(rotation[2])};

        rotation =
            rotation *
            math::rotate<float, 4>(dt * rotation_speed[0], direction_raw[0]) *
            math::rotate<float, 4>(dt * rotation_speed[1], direction_raw[1]) *
            math::rotate<float, 4>(dt * rotation_speed[2], direction_raw[2]);

        position += dt * math::transpose(direction_raw) * speed;
    }
};

#endif  // !ENTITY_H_
