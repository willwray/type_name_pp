project('type_name_pp', 'cpp', default_options : 'cpp_std=c++17')

subprojects = ['ntbs']

set_variable('ntbs_dep', subproject('ntbs').get_variable('ntbs_dep'))

test('sanity check type_name_pp',
  executable('sanity', 'test/sanity_check.cpp',
  dependencies : [ntbs_dep])
)

type_name_pp_dep = declare_dependency(
  include_directories : include_directories('.')
)
