project(
  'obdh2_sim',
  'c',
  version: '0.1.0',
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

zmq = dependency('libzmq')

libmop = subproject(
  'libmop',
  default_options: 'default_library=static',
)

libpredict = subproject(
  'libpredict',
  default_options: 'default_library=static',
)

obdh2_sim_deps += zmq
obdh2_sim_deps += libmop.get_variable('libmop_dep')
obdh2_sim_deps += libpredict.get_variable('libpredict_dep')

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
