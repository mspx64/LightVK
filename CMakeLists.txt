cmake_minimum_required(VERSION 3.16)
set(CMAKE_TOOLCHAIN_FILE
    "D:/dev/cpp/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING ""
)

project(LightVK VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(TO_CMAKE_PATH "$ENV{VULKAN_SDK}" VULKAN_SDK_PATH)

# Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_program(GLSLC glslc
    HINTS "$ENV{VULKAN_SDK}/Bin"
    NO_CMAKE_PATH
    NO_CMAKE_ENVIRONMENT_PATH
    REQUIRED
)

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/shaders")

file(GLOB SHADER_SOURCES
    "${SHADER_SOURCE_DIR}/*.vert"
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.comp" # Added .comp just in case you use compute shaders
)

set(COMPILED_SHADERS)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SHADER_OUTPUT "${SHADER_BINARY_DIR}/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        # Safely create directory during build stage, then compile
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
        COMMAND ${GLSLC} --target-env=vulkan1.3 --target-spv=spv1.6 ${SHADER} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${SHADER_NAME}"
        VERBATIM
    )

    list(APPEND COMPILED_SHADERS ${SHADER_OUTPUT})
endforeach()

# Create the master target containing all valid file paths
add_custom_target(compile_shaders ALL
    DEPENDS ${COMPILED_SHADERS}
)

# ---------------------------------------------------------
# 2. APPLICATION TARGET (Linked downstream)
# ---------------------------------------------------------
file(GLOB_RECURSE CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Gpu/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Gpu/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

set(VOLK_SOURCE
    "${VULKAN_SDK_PATH}/Include/Volk/volk.c"
)

add_executable(${PROJECT_NAME} ${CORE_SOURCES} ${VOLK_SOURCE})

# Now this dependency properly tracks the filled COMPILED_SHADERS list
add_dependencies(${PROJECT_NAME} compile_shaders)

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
    glfw
    glm::glm
    spdlog::spdlog
    fastgltf::fastgltf
    EnTT::EnTT
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${VOLK_DIR}
)
