add_library(ublib-box INTERFACE)

target_compile_features(ublib-box INTERFACE cxx_std_17)

target_link_libraries(ublib-box INTERFACE ublib-internal)

target_include_directories(ublib-box
  INTERFACE
    "include")

add_subdirectory("test-pimpl")
add_subdirectory("test-recursive")
