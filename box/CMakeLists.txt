add_library(ublib-box INTERFACE)

target_compile_features(ublib-box INTERFACE cxx_std_17)

add_subdirectory("test-pimpl")
add_subdirectory("test-recursive")

if (MSVC)
  target_compile_options(ublib-box
    INTERFACE
	  -permissive-)
endif()

target_include_directories(ublib-box
  INTERFACE
    "include")