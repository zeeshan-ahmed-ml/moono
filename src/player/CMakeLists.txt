add_library(player Player.h Player.cpp)

target_link_libraries(player card)

target_include_directories(player PUBLIC .)
