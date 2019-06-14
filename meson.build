project('type_name_pp', 'cpp', default_options : 'cpp_std=c++17')

src = []

test('test type_name_pp',
  executable('type_name_pp', 'test/type_name_pp.cpp')
)

type_name_pp_dep = declare_dependency(
  include_directories : include_directories('.')
)
