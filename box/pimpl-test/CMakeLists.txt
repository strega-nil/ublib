add_executable(pimpl_test_box source/main.cpp source/pimpl.cpp)

target_include_directories(pimpl_test_box
  PRIVATE
    source)

target_link_libraries(pimpl_test_box ublib_box)