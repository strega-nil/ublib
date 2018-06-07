add_executable(recursive_test_box source/main.cpp)

target_include_directories(recursive_test_box
  PRIVATE
    source)

target_link_libraries(recursive_test_box ublib_box)