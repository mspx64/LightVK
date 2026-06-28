file(GLOB_RECURSE EDITOR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(LightVK_Editor
    ${EDITOR_SOURCES}
)

add_dependencies(LightVK_Editor compile_shaders)

target_link_libraries(LightVK_Editor
    PRIVATE
        LightVKCore      # brings in all Core/Gpu/Vulkan + their public deps
        imgui::imgui     # editor-only, not needed by runtime
)

# ── Post-build: mirror shaders next to the binary ─────────────────
add_custom_command(TARGET LightVK_Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:LightVK_Editor>/shaders"
    COMMENT "Copying shaders → editor output"
)

# ── Post-build: mirror assets next to the binary ──────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET LightVK_Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:LightVK_Editor>/assets"
        COMMENT "Copying assets → editor output"
    )
endif()