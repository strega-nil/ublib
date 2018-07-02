add_library(ublib-integers INTERFACE)

target_compile_features(ublib-integers INTERFACE cxx_std_14)

target_link_libraries(ublib-integers INTERFACE ublib-internal)

target_include_directories(ublib-integers
  INTERFACE
    "include")

add_subdirectory("test-basic")