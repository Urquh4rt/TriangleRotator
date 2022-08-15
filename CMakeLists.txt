cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)

project ("CmakeTest")

include_directories(${CMAKE_SOURCE_DIR}/SDL/include include)
link_directories(${CMAKE_SOURCE_DIR}/SDL/lib)

add_executable (Main "main.cpp" "Coords.cpp" "FileSelector.cpp" "SDL_Interface.cpp" "TriangleBoard.cpp")
target_link_libraries(Main SDL2 SDL2main SDL2_image SDL2_ttf)
