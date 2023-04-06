all: bin/xorelf-interpreter bin/xorelf bin/xorelf-register

bin/xorelf-interpreter: src/xorelf-interpreter.sh
	cp $< $@
	sed -i 's|XORELF_BIN_PATH|$(realpath bin)|g' $@
	chmod +x bin/xorelf-interpreter

bin/xorelf-register: src/xorelf-register.sh
	cp $< $@
	sed -i 's|XORELF_BIN_PATH|$(realpath bin)|g' $@
	chmod +x bin/xorelf-register

bin/xorelf: src/*.c
	gcc $^ -o $@

