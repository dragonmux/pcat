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
	meson_version: '>= 0.48'
)

cxx = meson.get_compiler('cpp')

pcatSrcs = [
	'src/pcat.cxx', 'src/args.cxx', 'src/args/types.cxx', 'src/args/tokenizer.cxx'
]

executable(
	'pcat',
	pcatSrcs
)
