#ifndef CAMERA_H_
#define CAMERA_H_

#include "../utils/mat.h"
#include "../utils/mat_opengl.h"

struct Camera {
   public:
    math::vec3f position{};
    math::mat4f persp = math::identity<float, 4>();
    math::mat4f rotation = math::identity<float, 4>();

    // NOTE: This should be called AFTER position is set
    constexpr void lookAt(math::vec3f point) {
        auto v = math::normalize(point - position);

        // the aim is to enode a rotation that maps (0, 0, -1) onto
        // v; Or (0,0,1) onto -v.
        // We can achieve this by composing only two rotation, one along the x
        // axis and one along the y axix
        const math::vec3f x{1, 0, 0}, y{0, 1, 0}, z{0, 0, 1};
        v = -v;  // now we map (0, 0, 1) onto v

        // y rot
        auto v_proj_y = math::normalize(v - math::dot(v, y) * y);
        if(math::norm2(v_proj_y) == 0) v_proj_y = z;
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
        if(math::norm2(v_proj_x) == 0) v_proj_x = z;
        const auto v_perp_x = math::normalize(math::wedge(v_proj_x, x));

        const math::mat3f rot_x{{
            {1, 0, 0},
            {0, v_perp_x[1], v_perp_x[2]},
            {0, v_proj_x[1], v_proj_x[2]},
        }};

        rotation = math::extend<float, 3, 3, 4>(rot_x * rot_y);
    }

    [[nodiscard]] constexpr math::mat4f getCameraMatrix() const {
        // We move world by -transl, this the same effect as moving camera by
        // transl;
        auto transl =
            math::translation(-position[0], -position[1], -position[2]);
        return persp * rotation * transl;
    }
};

#endif  // !CAMERA_H
