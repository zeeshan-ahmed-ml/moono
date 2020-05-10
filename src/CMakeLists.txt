cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(public/src)

add_subdirectory(card)
add_subdirectory(player)
add_subdirectory(uno)
add_subdirectory(hashutil)
add_subdirectory(simplebitmap)

add_subdirectory(main)