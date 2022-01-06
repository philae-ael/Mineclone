#ifndef CAMERA_EVENT_H_
#define CAMERA_EVENT_H_

#include "../system/event_manager.h"
#include "../utils/mat.h"
#include "../utils/logging.h"

template <>
struct Event<struct CameraMoveEvent_> {
    math::vec3f translationAxis{};
    math::vec3f rotationAxis{};
};

using CameraMoveEvent = Event<struct CameraMoveEvent_>;

template <>
inline void trace_event(const CameraMoveEvent & event){
    auto logger = Logger::get({"CameraMoveEvent"});
    logger << "Translation Axis :"<< event.translationAxis << "rotationAxis :" << event.rotationAxis;
}

#endif  // !CAMERA_EVENT_H_
