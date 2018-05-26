Build project with CMake (minimum version 3.1).

Build steps:
```
cd <project_dir>
cmake -B./build -H. -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
```

For build debug version add:
```
cmake -B./build -H. -DCMAKE_BUILD_TYPE=Debug
```

For build on Windows with MinGW add:
```
cmake -B./build -H. -G"MinGW Makefiles"
```

Build tested with `GCC 5.4.0` and `MinGW 6.3.0`.
