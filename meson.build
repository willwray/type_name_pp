project('type_name_pp', 'cpp', default_options : 'cpp_std=c++17')

subprojects = ['ntbs']

foreach s : subprojects
    dep = s + '_dep'
    set_variable(dep, subproject(s).get_variable(dep))
endforeach

set_variable('ntbs_dep', subproject('ntbs').get_variable('ntbs_dep'))

test('test type_name_pp',
  executable('type_name_pp', 'test/type_name_pp.cpp',
  dependencies : [ntbs_dep])
)

type_name_pp_dep = declare_dependency(
  include_directories : include_directories('.')
)
