libcardboard_inc = include_directories('include')

expected_proj = subproject('expected', required: true)
expected = expected_proj.get_variable('expected_dep')

cereal_proj = subproject('cereal', required: true)
cereal = cereal_proj.get_variable('cereal_dep')

sources = files(
    'src/command_protocol.cpp',
    'src/ipc.cpp',
    'src/client.cpp',
)

install_subdir('include/cardboard',
               install_dir: get_option('includedir')
              )

libcardboard = library(
    'cardboard',
    sources,
    include_directories: libcardboard_inc,
    install: true,
    dependencies: [cereal, expected],
    cpp_args: '-Wno-deprecated'
)
