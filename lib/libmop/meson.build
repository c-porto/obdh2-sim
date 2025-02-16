project(
  'libmop',
  'c',
  version: '0.1.0',
  default_options: [
    'warning_level=3',
    'c_std=c99',
    'optimization=s',
    'buildtype=release',
    'default_library=both',
  ],
)

cc = meson.get_compiler('c')

mop_srcs = []

mop_inc = include_directories('include', 'src')

mop_deps = []

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

libmop = library(
  'mop',
  sources: mop_srcs,
  include_directories: mop_inc,
  dependencies: mop_deps,
  c_args: c_args,
  install: false,
  version: meson.project_version(),
  soversion: meson.project_version().split('.')[0],
  pic: true,
)

