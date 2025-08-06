cmake_minimum_required(VERSION 3.31)

project(proki CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "/home/tclere/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")

add_executable(proki
        src/service/ProcessServiceWindows.cpp
        src/service/ProcessService.h
        src/service/ProcessInfo.h
        src/service/ProcessServiceLinux.cpp
        src/service/ProcessServiceMac.cpp
        src/ui/process/ProcessUI.cpp
        src/ui/process/ProcessUI.h
        src/service/ProcessUpdater.cpp
        src/service/ProcessUpdater.h
        src/service/ProcessInfo.cpp)

target_sources(proki PRIVATE
        src/main.cpp src/app/App.cpp
        src/ui/Ui.cpp
)

target_include_directories(proki PRIVATE src)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(proki PRIVATE glfw imgui::imgui OpenGL::GL)