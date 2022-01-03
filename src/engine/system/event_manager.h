#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include <functional>
#include <vector>

template <class EventKind>
class Event {};

template <class EventKind>
using EventHandler = std::function<void(const Event<EventKind>&)>;

template <class EventKind>
void trace_event(const Event<EventKind> & /*event*/) {}

namespace EventManager {

template <class EventKind>
struct Storage {
    using storage_type = std::vector<EventHandler<EventKind>>;
    static storage_type* get_event_handlers() {
        static storage_type storage{};
        return &storage;
    }
};

template <class EventKind>
void subscribe(EventHandler<EventKind>&& handler) {
    auto handlers = Storage<EventKind>::get_event_handlers();
    handlers->push_back(std::move(handler));
}

template <class EventKind>
void dispatch(const Event<EventKind> &event) {
    auto handlers = Storage<EventKind>::get_event_handlers();

    trace_event(event);

    for (auto &handler : *handlers){
        handler(event);
    }
}

};  // namespace EventManager

#endif  // !EVENT_MANAGER_H_
