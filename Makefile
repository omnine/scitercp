.PHONY:buildx64debug
buildx64debug:
	-mkdir buildx64debug
	conan install . -s "compiler=msvc" -s arch=x86_64 -s build_type=Debug -s compiler.runtime=static -s compiler.version=194 -s compiler.toolset=v143 --build=missing --output-folder=buildx64debug
	cd buildx64debug
	cmake -S . -B buildx64debug -G "Visual Studio 17 2022" -A x64 -DCMAKE_GENERATOR_TOOLSET=v143 -DCMAKE_TOOLCHAIN_FILE="buildx64debug/build/generators/conan_toolchain.cmake"
	cmake --build buildx64debug --config Debug
	
	
