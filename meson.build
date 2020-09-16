project(
	'pcat',
	['cpp'],
	default_options: [
		'buildtype=release',
		'cpp_std=c++17',
		'warning_level=3',
		'b_ndebug=if-release',
		'b_lto=true',
		'strip=true'
	],
	license: 'GPL3+',
	version: '0.1.0',
	meson_version: '>= 0.52'
)

cxx = meson.get_compiler('cpp')
sanitizer = get_option('b_sanitize')
substrate = include_directories('substrate')
threading = dependency('threads')
buildDir = meson.current_build_dir()
srcDir = meson.current_source_dir()

fPIC = []
if cxx.has_argument('-fPIC')
	fPIC += ['-fPIC']
endif

if host_machine.system() == 'windows'
	add_project_arguments(
		'-wd4996',
		'-wd4848',
		'-Zc:__cplusplus',
		'-D_WINDOWS',
		'-DSUBSTRATE_BUILD_INTERNAL',
		language: 'cpp'
	)
endif

pcatSrcs = [
	'src/pcat.cxx', 'src/args.cxx', 'src/args/types.cxx', 'src/args/tokenizer.cxx',
	'substrate/impl/console.cxx',
	'src/algorithm/blockLinear/chunking.cxx',
	'src/algorithm/chunkSpans/chunking.cxx'
]
platformHeaders = include_directories('src/@0@'.format(host_machine.system()))

subdir('src')
pcat = executable(
	'pcat',
	pcatSrcs + [versionHeader],
	cpp_args: ['-D_GNU_SOURCE'] + fPIC,
	include_directories: [substrate, include_directories('src'), platformHeaders],
	dependencies: [threading],
	gnu_symbol_visibility: 'inlineshidden',
	implicit_include_directories: false,
	install: true,
	pie: target_machine.system() != 'windows'
)

crunchMake = find_program('crunchMake', version: ['>= 1.0.1', '< 2.0.0'], required: false)
subdir('test', if_found: crunchMake)

runClangTidy = find_program('runClangTidy.py')
run_target(
	'clang-tidy',
	command: [runClangTidy, '-s', meson.current_source_dir(), '-p', meson.current_build_dir()]
)

subdir('man')
