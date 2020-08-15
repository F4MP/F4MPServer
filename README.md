# F4MPServer

## Build Instructions

Make sure you have all the dependencies installed and in the correct locations for includes and linking.

With make and g++/clang compilation will look like this

```
mkdir build
cd build
cmake ..
make
```

With Visual Studio, in the launcher "Continue Without Code" -> File -> Open -> CMake
Intellisense should just do it's thing and read the CMakeSettings.json file for build and compiler settings with Ninja 

