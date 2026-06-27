file(GLOB_RECURSE EDITOR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(RenderX_Editor
    ${EDITOR_SOURCES}
    "${CMAKE_SOURCE_DIR}/src/Game.cpp"   # entry point (main)
)

add_dependencies(RenderX_Editor compile_shaders)

target_link_libraries(RenderX_Editor
    PRIVATE
        RenderXCore      # brings in all Core/Gpu/Vulkan + their public deps
        imgui::imgui     # editor-only, not needed by runtime
)

# ── Post-build: mirror shaders next to the binary ─────────────────
add_custom_command(TARGET RenderX_Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:RenderX_Editor>/shaders"
    COMMENT "Copying shaders → editor output"
)

# ── Post-build: mirror assets next to the binary ──────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET RenderX_Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:RenderX_Editor>/assets"
        COMMENT "Copying assets → editor output"
    )
endif()