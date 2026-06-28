file(GLOB_RECURSE RUNTIME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable(LightVK_Runtime
    ${RUNTIME_SOURCES}
)

add_dependencies(LightVK_Runtime compile_shaders)

target_link_libraries(LightVK_Runtime
    PRIVATE
        LightVKCore
)

# ── Post-build: mirror shaders next to the binary ─────────────────
add_custom_command(TARGET LightVK_Runtime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:LightVK_Runtime>/shaders"
    COMMENT "Copying shaders → runtime output"
)

# ── Post-build: mirror assets next to the binary ──────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    add_custom_command(TARGET LightVK_Runtime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:LightVK_Runtime>/assets"
        COMMENT "Copying assets → runtime output"
    )
endif()
