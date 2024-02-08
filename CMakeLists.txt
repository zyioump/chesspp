cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

project(chesspp)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_ttf REQUIRED)

set(SOURCE_LIST
    src/user_interface.cpp
    src/board.cpp
    src/moves/generator.cpp
    src/moves/nonSlidingPieces.cpp
    src/moves/slidingPieces.cpp
    src/utils.cpp
    src/ai/ai.cpp
    src/ai/quiesce.cpp
    src/ai/search.cpp
    src/ai/evaluation.cpp
    src/ai/piece_table.cpp
    src/zobrist.cpp
    src/transposition_table.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCE_LIST})  

add_executable(perft src/perft.cpp ${SOURCE_LIST})
add_executable(enginet src/enginet.cpp ${SOURCE_LIST})

target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image SDL2::GFX SDL2::TTF)
target_include_directories(${PROJECT_NAME} PRIVATE includes/)

target_link_libraries(perft SDL2::Main SDL2::Image SDL2::GFX SDL2::TTF)
target_include_directories(perft PRIVATE includes/)

target_link_libraries(enginet SDL2::Main SDL2::Image SDL2::GFX SDL2::TTF)
target_include_directories(enginet PRIVATE includes/)

