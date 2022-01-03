#ifndef CAMERA_H_
#define CAMERA_H_

#include "../utils/mat.h"
#include "../utils/mat_opengl.h"

struct Camera {
   public:
    math::vec3f position{};
    math::mat4f persp = math::identity<float, 4>();
    math::mat4f rotation = math::identity<float, 4>();
};

#endif  // !CAMERA_H
