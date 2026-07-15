#pragma once

#include "Editor/Context.h"
#include "Editor/Panels/Hierarchy.h"

struct GLFWwindow;

namespace Lgt {

namespace Editor {
class Editor {
public:
    void Init(World* world);
    void Update();
    void Shutdown();
private:
    Context          context_;
    Panel::Hierarchy panel_hirearchy_;
};

} // namespace Editor

} // namespace Lgt
