add_library(ublib-internal INTERFACE)

target_compile_features(ublib-internal INTERFACE cxx_std_14)

if (MSVC)
  target_compile_options(ublib-internal
    INTERFACE
	  -permissive-)
endif()

target_include_directories(ublib-internal
  INTERFACE
    "include")