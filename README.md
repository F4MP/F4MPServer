# F4MPServer

## Build Instructions

### Windows

#### Dependencies 

[SLikeNet](https://cdn.discordapp.com/attachments/734738308521001000/744612603703066684/raknet_dist.zip)

#### Building

Make sure you have all the dependencies installed and in the correct locations for includes and linking.

With Visual Studio, in the launcher "Continue Without Code" -> File -> Open -> CMake.
Intellisense should just do it's thing and read the CMakeSettings.json file for build and compiler settings with Ninja 

With CLion, all you need to do is Open Project and navigate to the CMakeList and open it with your favorite toolchain

### Linux

#### Dependencies

Install slikenet through Vcpkg
 
#### Building

With cmake/make and g++/clang compilation will look like this

```
mkdir build
cd build
cmake ..
make
```
