#ifndef CAMERA_H_
#define CAMERA_H_

#include "../utils/mat.h"
#include "../utils/mat_opengl.h"
#include "entity.h"

struct Camera {
   public:
    Entity self{};
    Entity* parent = nullptr;

    math::mat4f persp = math::identity<float, 4>();
};

#endif  // !CAMERA_H
