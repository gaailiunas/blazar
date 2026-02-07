add_executable(control_plane src/main.c)
target_include_directories(control_plane PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(control_plane PRIVATE common)
