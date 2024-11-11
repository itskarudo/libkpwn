# we set this manually because libgc checks using the system's libc instead of musl so idk
set(HAVE_GETCONTEXT FALSE)

set(lib_sources
  bytes.c
  encoding.c
  io.c
  kelf.c
  kpwn.c
  log.c
  utils.c
)

set(lib_headers
  bytes.h
  encoding.h
  io.h
  kelf.h
  kpwn.h
  log.h
  utils.h
)

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(deps)
add_library(libkpwn STATIC ${lib_sources} ${lib_headers})

set_property(TARGET libkpwn PROPERTY OUTPUT_NAME kpwn)

set_target_properties(libkpwn PROPERTIES
                      PUBLIC_HEADER "${lib_headers}"
                      )

target_include_directories(libkpwn PUBLIC . deps)
target_link_libraries(libkpwn PUBLIC gc)

target_compile_options(libkpwn PRIVATE -Wall -Wextra)

install(TARGETS libkpwn EXPORT libkpwn-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kpwn
)
