cmake_minimum_required(VERSION 3.12)

project(BlueMarble)

add_executable(BlueMarble main.cpp )

target_include_directories(BlueMarble PRIVATE deps/glm 
                                              deps/glfw/include
                                              deps/glew/include
                                              deps/stb)

target_link_directories(BlueMarble PRIVATE deps/glfw/lib-vc2019
                                           deps/glew/lib/Release/x64)

target_link_libraries(BlueMarble PRIVATE glfw3.lib glew32.lib opengl32.lib)

add_custom_command(TARGET BlueMarble POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/glew/bin/Release/x64/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll"
                   COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders"
                   COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_SOURCE_DIR}/textures" "${CMAKE_BINARY_DIR}/textures"
)

add_executable(Vetores Vetores.cpp )
target_include_directories(Vetores PRIVATE deps/glm)

add_executable(Matrizes Matrizes.cpp )
target_include_directories(Matrizes PRIVATE deps/glm)