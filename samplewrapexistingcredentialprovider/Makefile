.PHONY:buildx64
buildx64:
	-mkdir ..\buildx64
	cd ..\buildx64 && conan install .. -s "compiler=Visual Studio" -s arch=x86_64 -s build_type=Release -s compiler.runtime=MT -s compiler.version=17 -s compiler.toolset=v142 --build=missing
	cmake -S .. -B ../buildx64 -G "Visual Studio 17 2022" -A x64 -DCMAKE_GENERATOR_TOOLSET=v142
	cmake --build ../buildx64 --config Release
	
	
.PHONY:buildarm64
buildarm64:
	-mkdir ..\buildarm64
	cd ..\buildarm64 && conan install .. -s "compiler=Visual Studio" -s arch=armv8 -s build_type=Release -s compiler.runtime=MT -s compiler.version=17 -s compiler.toolset=v143 --build=missing
	cmake -S .. -B ../buildarm64 -G "Visual Studio 17 2022" -A armv8 -DCMAKE_GENERATOR_TOOLSET=v143
	cmake --build ../buildarm64 --config Release	

