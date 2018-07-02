add_executable(ublib-box-test-recursive src/main.cpp)

target_include_directories(ublib-box-test-recursive
  PRIVATE
    src)

target_link_libraries(ublib-box-test-recursive ublib_box)