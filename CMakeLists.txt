cmake_minimum_required(VERSION 3.0.0)
project(HikvisionCCD-opencv VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
include_directories(${PROJECT_SOURCE_DIR}/include)

IF(WIN32)
    SET(DCMAKE_TOOLCHAIN_FILE "D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
    include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
ENDIF()

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)

find_package(OpenCV 4.5.5 REQUIRED)

aux_source_directory(./src SrcFiles)
set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)
add_executable(main ${SrcFiles})

target_link_libraries(main ${OpenCV_LIBS} MvCameraControl)
