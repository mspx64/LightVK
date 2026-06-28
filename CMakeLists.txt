cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE
    "D:/dev/cpp/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING ""
)

project(LightVK VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" VULKAN_SDK_PATH)

# ── Packages ──────────────────────────────────────────────────────
find_package(Vulkan   REQUIRED)
find_package(glfw3    CONFIG REQUIRED)
find_package(glm      CONFIG REQUIRED)
find_package(spdlog   CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)
find_package(EnTT     CONFIG REQUIRED)
find_package(imgui    CONFIG REQUIRED)

# ── Shader compilation ────────────────────────────────────────────
find_program(GLSLC glslc
    HINTS "$ENV{VULKAN_SDK}/Bin"
    NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH
    REQUIRED
)

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/shaders")

file(GLOB SHADER_SOURCES
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.comp"
)

set(COMPILED_SHADERS)
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUTPUT "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv")
    add_custom_command(
        OUTPUT  ${SHADER_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        COMMAND ${GLSLC} --target-env=vulkan1.3 --target-spv=spv1.6 ${SHADER} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${SHADER_NAME}"
        VERBATIM
    )
    list(APPEND COMPILED_SHADERS ${SHADER_OUTPUT})
endforeach()

add_custom_target(compile_shaders ALL DEPENDS ${COMPILED_SHADERS})

# ── Volk — shared between Core and Editor ─────────────────────────
set(VOLK_SOURCE "${VULKAN_SDK_PATH}/Include/Volk/volk.c"
    CACHE STRING "Volk loader source path")

# ── Sub-projects ──────────────────────────────────────────────────
add_subdirectory(src/Core)    # → static lib   LightVKCore
add_subdirectory(src/Editor)  # → executable   LightVK_Editor
add_subdirectory(src/Runtime) # → executable   LightVK_Runtime