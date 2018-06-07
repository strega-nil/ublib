add_executable(test_box source/main.cpp source/pimpl.cpp)

target_include_directories(test_box
  PRIVATE
    source)

target_link_libraries(test_box ublib_box)