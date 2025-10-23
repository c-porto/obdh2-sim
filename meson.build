project(
  'obdh2-sim',
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

m_dep = cc.find_library('m', required : false)

obdh2_sim_srcs = []

obdh2_sim_inc = include_directories('include', 'src')

obdh2_sim_deps = []

zmq = dependency('libzmq')

obdh2_sim_deps += m_dep

libmop = subproject(
  'libmop',
  default_options: 'default_library=static',
)

libpredict = subproject(
  'libpredict',
  default_options: 'default_library=static',
)

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

install_data('services/obdh2-sim.service',
             install_dir: get_option('systemd_system_unitdir'),
             rename: 'obdh2-sim.service')
