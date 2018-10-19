project(
  'polyvoid',
  'cpp',
  version: '0.1.0',
  license: 'Apache-2.0',
  default_options: [
    'werror=true',
    'warning_level=3',
    'cpp_std=c++17',
    'default_library=static'])

_compiler = meson.get_compiler('cpp').get_id()
if _compiler == 'msvc'
  add_project_arguments('-permissive-', language: 'cpp')

  # note: Meson's handling of C++17 on MSVC is borked
  add_project_arguments('-std:c++17', language: 'cpp')
endif

subdir('internal')
subdir('integers')
subdir('box')


