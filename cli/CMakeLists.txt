set (CMAKE_CXX_STANDARD 20)

set(cli_sources
  main.cpp
  cmds/init.cpp)

add_subdirectory(deps)

add_executable(kpwn ${cli_sources})
target_link_libraries(kpwn PRIVATE libkpwn argparse)

target_compile_options(kpwn PRIVATE -Wall -Wextra)

install(TARGETS kpwn RUNTIME DESTINATION bin)
