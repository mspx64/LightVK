#pragma once
#include "World.h"

namespace Lgt {

class Entity {

public:
    Entity() = default;
    Entity(entt::entity handle, World* world)
        : handle_(handle),
          world_(world) {}

    template <typename T, typename... Args> T& add(Args&&... args);
    template <typename T> T&                   remove();
    template <typename T> T&                   get();
    template <typename T> const T&             get() const;
    template <typename T> const bool           has() const;

    entt::entity handle() { return handle_; }
    bool         operator==(const Entity& o) const { return handle_ == o.handle_; }
    bool         operator!=(const Entity& o) const { return !(*this == o); }

private:
    entt::entity handle_ = entt::null;
    World*       world_  = nullptr;
};

//-------------------------------------------------------------------------------
template <typename T, typename... Args> inline T& Entity::add(Args&&... args) {
    return world_->registry().emplace<T>(handle_, std::forward<Args>(args)...);
}

template <typename T> inline T& Entity::remove() {
    return world_->registry().remove<T>(handle_);
}

template <typename T> inline T& Entity::get() {
    return world_->registry().get<T>(handle_);
}

template <typename T> inline const T& Entity::get() const {
    return world_->registry().get<T>(handle_);
}

template <typename T> inline const bool Entity::has() const {
    return world_->registry().all_of<T>(handle_);
}
//-------------------------------------------------------------------------------

} // namespace Lgt
