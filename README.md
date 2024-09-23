# Simple-NES-Emulator
Project aiming to run Super Mario Bros.

## Goals
The main goal of this project is to create a NES emulator without GUI.
A side goal is to try unit testing in C++ with [Google Test](https://github.com/google/googletest).
Once I'll achieve the main goal, maybe I'll consider doing a GUI.

## How to build
Run the following command :
```shell
cmake -B build
```
This will generate a makefile or a Visual Studio project (depending on your system).
Then, you can either build the main target of the test target.

### Compile on Linux
```shell
cd build

# Build targets one-by-one
make simple-nes-emulator
make simple-nes-emulator-TEST

# Build everything
make all
```

### Compile on Window (MSVC)
```batch
:: Build targets one-by-one
MSBuild.exe simple-nes-emulator.vcxproj /property:Configuration = Release
MSBuild.exe simple-nes-emulator-TEST.vcxproj /property:Configuration = Release
```

## Plan
- [ ] Emulate the CPU
- [ ] Learn more about the rest
	- [ ] PPU ?
	- [ ] Sound ?
	- [ ] Controller ?
	- [ ] Something else ?
- [ ] Set Up OpenGL for graphics rendering
- [ ] Use OpenAL for audio ?

## References
### Inspiration
Inspired by [Dave Poo](https://www.youtube.com/@DavePoo):
[Emulating a CPU in C++ (6502)](https://www.youtube.com/watch?v=qJgsuQoy9bc)
[Emulating a CPU in C++ #2 (6502) - Unit Testing
](https://youtu.be/L7J1pPokEyw?si=gRlonH_mJQViYa5g)

### 6502
[Andrew Jacobs - 6502](https://web.archive.org/web/20210727210256/http://obelisk.me.uk/6502/index.html)
[C64 Reset Process](https://www.c64-wiki.com/wiki/Reset_(Process)), I need to verify if it is different on the NES...

### Test
[Google Test](https://github.com/google/googletest)

## Fun fact
The CPU and its memory should fit in any modern computer stack.

## License

MIT License. See [LICENSE](LICENSE) file.