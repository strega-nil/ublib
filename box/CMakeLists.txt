add_library(ublib_box INTERFACE)

target_compile_features(ublib_box INTERFACE cxx_std_17)

add_subdirectory("test")

if (MSVC)
  target_compile_options(ublib_box
    INTERFACE
	  -permissive-)
endif()

target_include_directories(ublib_box
  INTERFACE
    "include")