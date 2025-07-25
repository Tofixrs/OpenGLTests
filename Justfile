builddir := 'build'

configure target='debug' *FLAGS='':
	cmake -GNinja -B {{builddir}} \
		-DCMAKE_BUILD_TYPE={{ if target == "debug" { "Debug" } else { "RelWithDebInfo" } }} \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON {{FLAGS}}

	ln -sf {{builddir}}/compile_commands.json compile_commands.json

_configure_if_clean:
	@if ! [ -d {{builddir}} ]; then just configure; fi

build: _configure_if_clean
	cmake --build {{builddir}}

release: (configure "release") build

clean:
	rm -f compile_commands.json
	rm -rf {{builddir}}

run *ARGS='': build
	{{builddir}}/src/app {{ARGS}}

