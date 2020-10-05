cmake_minimum_required (VERSION 3.13)
project(nixstore-c VERSION 0.0.1 DESCRIPTION "tiny C wraper for libnixstore which is C++")
add_library(nixstore-c SHARED lib.cpp)
set_target_properties(nixstore-c PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(nixstore-c PROPERTIES SOVERSION 1)
set_target_properties(nixstore-c PROPERTIES PUBLIC_HEADER nixstore-c.h)

find_package(PkgConfig REQUIRED)
pkg_check_modules(NIXSTORE REQUIRED nix-store)
pkg_check_modules(NIXEXPR REQUIRED nix-expr)

target_link_libraries(nixstore-c ${NIXSTORE_LIBRARIES} ${NIXEXPR_LIBRARIES} -lpthread)
target_include_directories(nixstore-c PUBLIC ${NIXSTORE_INCLUDE_DIRS} ${NIXEXPR_INCLUDE_DIRS})
target_compile_options(nixstore-c PUBLIC ${NIXSTORE_CFLAGS_OTHER} ${NIXEXPR_CFLAGS_OTHER})

configure_file(nixstore-c.pc.in nixstore-c.pc @ONLY)
include(GNUInstallDirs)
install(TARGETS nixstore-c
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/nixstore-c.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
