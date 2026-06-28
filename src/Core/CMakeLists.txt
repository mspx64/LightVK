file(GLOB_RECURSE CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(LightVKCore SHARED
    ${CORE_SOURCES}
    ${VOLK_SOURCE}
)

target_include_directories(LightVKCore
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
    "${VULKAN_SDK_PATH}/Include"
)

target_link_libraries(LightVKCore
    PUBLIC
    Vulkan::Vulkan
    glfw
    glm::glm
    spdlog::spdlog
    fastgltf::fastgltf
    EnTT::EnTT
)

target_compile_definitions(LightVKCore
    PRIVATE
    LIGHTVK_CORE_BUILD
    PUBLIC
    VK_NO_PROTOTYPES # Volk owns all Vulkan function pointers
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_RADIANS
)
