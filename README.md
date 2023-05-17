# ashe
ashe is a C++ common library.

# How to use
Clone repository and submodules:

```bash
git clone https://github.com/winsoft666/ashe.git
cd ashe
git submodule update --init --recursive
```

and include `ashe/include` directory to your project, then `#include` any ashe header file or `ashe.hpp`:

```c++
#include "ashe/file.hpp"
// or include all header files at once
#include "ashe.hpp"
```

# Build Test

```bash
git clone https://github.com/winsoft666/ashe.git
cd ashe
git submodule update --init --recursive

cd test
mkdir build
cd build
cmake ..
make
./test
```
