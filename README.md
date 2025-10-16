# ashe - C++ Common Library
ashe 是一个开源的 C++ 通用基础代码库，支持 Windows、Linux、macOS 等平台。

ashe 虽然没有 Boost、Abseil、Folly 这些库大而全，但胜在小而美。

# 编译
编译 ashe 不需要额外编译其他任何第三方库。

## vcpkg
```bash
vcpkg install ashe
```

## CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
