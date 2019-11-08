project(
	'pcat',
	['cpp'],
	default_options: [
		'buildtype=release',
		'cpp_std=c++14',
		'warning_level=3'
	],
	license: 'GPL3+',
	version: '0.0.1',
	meson_version: '>= 0.48'
)

cxx = meson.get_compiler('cpp')

pcatSrcs = [
	'pcat.cxx'
]

executable(
	'pcat',
	pcatSrcs
)
