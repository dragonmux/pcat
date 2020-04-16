project(
	'pcat',
	['cpp'],
	default_options: [
		'buildtype=release',
		'cpp_std=c++17',
		'warning_level=3'
	],
	license: 'GPL3+',
	version: '0.0.1',
	meson_version: '>= 0.52'
)

cxx = meson.get_compiler('cpp')
sanitizer = get_option('b_sanitize')
substrate = include_directories('substrate')

pcatSrcs = [
	'src/pcat.cxx', 'src/args.cxx', 'src/args/types.cxx', 'src/args/tokenizer.cxx', 'substrate/impl/console.cxx'
]

subdir('src')
pcat = executable(
	'pcat',
	pcatSrcs + [versionHeader],
	cpp_args: ['-D_GNU_SOURCE'],
	include_directories: [substrate, include_directories('src')],
	gnu_symbol_visibility: 'inlineshidden',
	implicit_include_directories: false,
	install: true,
	pie: true
)

crunchMake = find_program('crunchMake', required: false)
subdir('test', if_found: crunchMake)
