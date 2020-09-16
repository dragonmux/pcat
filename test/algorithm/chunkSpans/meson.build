chunkSpansTests = [
	'testChunkState', 'testFileChunker', 'testChunking'
]

algorithmTestHelpers = static_library(
	'testHelpers',
	[
		'chunkState.cxx', 'fileChunker.cxx'
	],
	pic: true,
	dependencies: [libcrunchpp],
	include_directories: [include_directories('../../../src'), substrate],
	install: false,
	build_by_default: true
)

testObjectMap = {
	'testChunkState': {'test': ['chunkState.cxx']},
	'testFileChunker': {'test': ['fileChunker.cxx']},
	'testChunking': {
		'pcat': [
			'src/algorithm/chunkSpans/chunking.cxx', 'src/args.cxx', 'src/args/tokenizer.cxx', 'src/args/types.cxx',
			'substrate/impl/console.cxx'
		]
	}
}

foreach test : chunkSpansTests
	map = testObjectMap.get(test, {})
	pcatObjs = map.has_key('pcat') ? [pcat.extract_objects(map['pcat'])] : []
	testObjs = map.has_key('test') ? [algorithmTestHelpers.extract_objects(map['test'])] : []
	testLibs = map.get('libs', [])
	custom_target(
		test,
		command: [
			crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I@0@/src'.format(srcDir)
		] + commandExtra + testLibs,
		input: [test + '.cxx'] + pcatObjs + testObjs,
		output: test + '.so',
		build_by_default: true
	)

	test(
		'chunkSpans-@0@'.format(test),
		crunchpp,
		args: [test],
		workdir: meson.current_build_dir(),
		is_parallel: false
	)
endforeach
