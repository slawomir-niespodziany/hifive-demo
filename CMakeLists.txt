cmake_minimum_required(VERSION 3.28)

project(demo 
    VERSION 1.0 
    LANGUAGES CXX ASM
)

add_executable(demo 
    main.cpp
    render.s
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(demo ${OpenCV_LIBS})

#target_include_directories(demo PRIVATE "/usr/local/include/opencv4")
#target_link_libraries(demo opencv_imgcodecs opencv_core opencv_highgui)
