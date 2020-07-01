pandoc = find_program('pandoc', required: false)

if pandoc.found()
	manSrcs = {
		'pcat.1.md': 'pcat.1',
	}
	manuals = []
	foreach manSrc, manOut : manSrcs
		manuals += [configure_file(
			command: [pandoc, '-s', '-t', 'man', '-o', '@OUTPUT@', '@INPUT@'],
			input: manSrc,
			output: manOut,
			install: false
		)]
	endforeach
else
	warning('A suitable version of `pandoc` was not found; ' +
		'changes to man/*.md will not affect your build.')
	manuals = files('pcat.1')
endif

install_man(manuals)
