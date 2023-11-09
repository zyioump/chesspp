cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

project(chesspp)

set(CMAKE_BUILD_TYPE debug)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/user_interface.cpp
    src/board.cpp
    src/moves/pawn.cpp
    src/moves/knight.cpp
    src/moves/king.cpp
    src/moves/slidingPieces.cpp
    src/utils.cpp
)  

target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image SDL2::GFX)
target_include_directories(${PROJECT_NAME} PRIVATE includes/)
