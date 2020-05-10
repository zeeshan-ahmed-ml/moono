add_library(uno Uno.h Uno.cpp)

target_link_libraries(uno player card simplebitmap hashutil fslazywindow ysbitmapfont)

target_include_directories(uno PUBLIC .)
