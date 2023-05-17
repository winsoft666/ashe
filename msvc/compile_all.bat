@echo off
@echo Compiling "Debug_MD|x86"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Debug_MD|x86" ./ashe.sln

@echo Compiling "Release_MD|x86"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Release_MD|x86" ./ashe.sln

@echo Compiling "Debug_MT|x86"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Debug_MT|x86" ./ashe.sln

@echo Compiling "Release_MT|x86"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Release_MT|x86" ./ashe.sln

@echo Compiling "Debug_MD|x64"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Debug_MD|x64" ./ashe.sln

@echo Compiling "Release_MD|x64"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Release_MD|x64" ./ashe.sln

@echo Compiling "Debug_MT|x64"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Debug_MT|x64" ./ashe.sln

@echo Compiling "Release_MT|x64"
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /build "Release_MT|x64" ./ashe.sln

echo All Compile OK!
pause