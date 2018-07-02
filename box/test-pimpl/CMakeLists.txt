add_executable(ublib-box-test-pimpl src/main.cpp src/pimpl.cpp)

target_include_directories(ublib-box-test-pimpl
  PRIVATE
    src)

target_link_libraries(ublib-box-test-pimpl ublib-box)