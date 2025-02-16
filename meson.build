project(
  'obdh2_sim',
  'c',
  version: '0.0.5',
  default_options: [
    'warning_level=3',
    'c_std=gnu11',
    'optimization=3',
    'buildtype=release',
  ],
)

cc = meson.get_compiler('c')

obdh2_sim_srcs = []

obdh2_sim_inc = include_directories('include', 'src')

obdh2_sim_deps = []

sqlite3 = dependency('sqlite3')

zmq = dependency('libzmq')

libmop = subproject(
  'libmop',
  default_options: 'default_library=static',
)

obdh2_sim_deps += zmq
obdh2_sim_deps += sqlite3
obdh2_sim_deps += libmop.get_variable('libmop_dep')

c_args = [
  '-Wno-unused-parameter',
  '-Wno-unused-value',
  '-Wno-missing-field-initializers',
  '-Wno-narrowing',
  '-Wno-pointer-arith',
  '-Wshadow',
  '-Wcast-align',
  '-Wwrite-strings',
]

subdir('src')

obdh2_sim = executable(
  meson.project_name(),
  sources: obdh2_sim_srcs,
  include_directories: obdh2_sim_inc,
  dependencies: obdh2_sim_deps,
  c_args: c_args,
  install: true,
)

inst_srcs = get_option('install_src')
inst_srcs_dir = get_option('src_install_dir')

if inst_srcs
  install_subdir('include', install_dir: inst_srcs_dir)
  install_subdir('src', install_dir: inst_srcs_dir)
  install_subdir('subprojects', install_dir: inst_srcs_dir)
  install_data('meson.build', install_dir: inst_srcs_dir)
endif
