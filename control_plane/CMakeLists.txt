add_executable(control_plane src/main.c src/server.c src/client.c)
target_link_libraries(control_plane PRIVATE common)
