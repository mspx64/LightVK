#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Core/Logger.h"
#include "Vulkan/Helpers.h"

struct PassNode {
    std::string name;
};

class RenderGraph {};