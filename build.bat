@echo off
cmake -G "Visual Studio 17 2022" ^
      -T "v142" ^
      -A "Win32" ^
      -DBUILD_TESTS=OFF ^
      -DASHE_BUILD_SHARED_LIBS=ON ^
      -DUSE_STATIC_CRT=OFF ^
      -DCMAKE_INSTALL_PREFIX=%~dp0\msvc\lib\x86\md\shared\ ^
      -S %~dp0 -B %~dp0build

cmake --build %~dp0build
cmake --install %~dp0build

pause