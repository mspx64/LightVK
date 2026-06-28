#pragma once
#include "World.h"

namespace Lgt {

class Entity {

public:
    Entity() = default;
    Entity(entt::entity handle, World* world)
        : handle_(handle),
          world_(world) {}

    template <typename T, typename... Args> T& Add(Args&&... args);
    template <typename T> T&                   Remove();
    template <typename T> T&                   Get();
    template <typename T> const T&             Get() const;
    template <typename T> const bool           Has() const;

    entt::entity Handle() { return handle_; }
    bool         operator==(const Entity& o) const { return handle_ == o.handle_; }
    bool         operator!=(const Entity& o) const { return !(*this == o); }

private:
    entt::entity handle_ = entt::null;
    World*       world_  = nullptr;
};

//-------------------------------------------------------------------------------
template <typename T, typename... Args> inline T& Entity::Add(Args&&... args) {
    return world_->Registry().emplace<T>(handle_, std::forward<Args>(args)...);
}

template <typename T> inline T& Entity::Remove() {
    return world_->Registry().Remove<T>(handle_);
}

template <typename T> inline T& Entity::Get() {
    return world_->Registry().get<T>(handle_);
}

template <typename T> inline const T& Entity::Get() const {
    return world_->Registry().get<T>(handle_);
}

template <typename T> inline const bool Entity::Has() const {
    return world_->Registry().all_of<T>(handle_);
}
//-------------------------------------------------------------------------------

} // namespace Lgt
