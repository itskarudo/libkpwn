cmake_minimum_required(VERSION 3.12)
project(kpwn LANGUAGES C CXX VERSION 0.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

add_subdirectory(lib)
add_subdirectory(cli)

