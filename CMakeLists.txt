cmake_minimum_required(VERSION 3.12)
project(LibKPWN)

# we set this manually because libgc checks using the system's libc instead of musl so idk
set(HAVE_GETCONTEXT FALSE)

# override default build type
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_subdirectory(deps)

set(SOURCES
  src/assertions.c
  src/bytes.c
  src/encoding.c
  src/io.c
  src/kelf.c
  src/kpwn.c
  src/log.c
  src/utils.c
)

add_library(kpwn STATIC ${SOURCES})
target_include_directories(kpwn PUBLIC src/ deps/)
target_link_libraries(kpwn PUBLIC gc)

target_compile_options(kpwn PRIVATE -Wall -Wextra -Wpedantic -Werror)
