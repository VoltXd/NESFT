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
MSBuild.exe simple-nes-emulator.vcxproj /property:Configuration=Release
MSBuild.exe simple-nes-emulator-TEST.vcxproj /property:Configuration=Release
```

## Plan
- [x] CPU
	- [x] Official instructions
	- [ ] Illegal instructions (not planned to do them)
- [x] PPU
	- [x] Rendering with OpenGL
	- [x] Pixels rendering
		- [x] Background rendering
		- [x] Sprites rendering
	- [ ] Verify 8x16 sprite rendering (one day)
	- [ ] Color emphasis & greyscale
- [x] Controller
- [ ] APU
	- [x] Stream audio with OpenAL
	- [ ] Fix OpenAL
		- [x] Sound latency
		- [ ] No sound if app closed unexpectedly...
	- [x] Generate sound signals
		- [x] Pulses
		- [x] Triangle
		- [x] Noise
		- [ ] DMC (implemented but need more verification)
- [ ] Mappers
	- [x] 000 (SMB, DK, Excitebike...)
	- [ ] Total: **1** / 255 iNES 1.0 mappers
- [ ] Rearrange classes members (for alignment (unnecessary optimisation))

## References
### Inspiration
Inspired by [Dave Poo](https://www.youtube.com/@DavePoo):\
[Emulating a CPU in C++ (6502)](https://www.youtube.com/watch?v=qJgsuQoy9bc)\
[Emulating a CPU in C++ #2 (6502) - Unit Testing
](https://youtu.be/L7J1pPokEyw?si=gRlonH_mJQViYa5g)

And [OneLoneCoder](https://www.youtube.com/@javidx9)\
[NES Emulator from scratch](https://www.youtube.com/playlist?list=PLrOv9FMX8xJHqMvSGB_9G9nZZ_4IgteYf)

### NES Hardware
[NESdev Wiki](https://www.nesdev.org/wiki/NES_reference_guide)\
[Palette color](https://forums.nesdev.org/viewtopic.php?t=746)

### 6502
[Andrew Jacobs - 6502](https://www.nesdev.org/obelisk-6502-guide/index.html)\
[C64 Reset Process](https://www.c64-wiki.com/wiki/Reset_(Process)), I need to verify if it is different on the NES...\
[6502 Overflow flag explained](https://www.righto.com/2012/12/the-6502-overflow-flag-explained.html)

### Test
[Google Test](https://github.com/google/googletest)

### OpenAL
[OpenAL Documentation](https://www.openal.org/documentation/)\
[OpenAL destroy computer audio FIX](https://stackoverflow.com/questions/9397681/openal-randomly-stops-playing-some-sounds-can-only-fix-with-reboot)\
[Sound latency (just don't use 165463 sound buffers please...)](https://en.sfml-dev.org/forums/index.php?topic=3929.0)

## License

MIT License. See [LICENSE](LICENSE) file.