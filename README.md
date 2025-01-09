# NESFT
Nes Emulator with Sound Fourier Transform.
A project aiming to run Super Mario Bros. (and more...)

## Goals
The main goal of this project was to create a NES emulator without GUI.
A side goal was to try unit testing in C++ with [Google Test](https://github.com/google/googletest).
Once I achieved the main goal, I started to make a GUI including mainly sound visualisation features.

## How to build
First, clone this repository:
```shell
git clone --recursive https://github.com/VoltXd/NESFT.git
# or
git clone --recursive git@github.com:VoltXd/NESFT.git
```
Run the following command :
```shell
cmake -B build
```
This will generate a makefile or a Visual Studio project (depending on your system).
Then, you can either build the main target or the test target.

### Compile on Linux
```shell
cd build

# Build targets one-by-one
make nesft
make nesft-TEST

# Build everything
make all
```

### Compile on Window (MSVC)
```batch
:: Build targets one-by-one
MSBuild.exe nesft.vcxproj /property:Configuration=Release
MSBuild.exe nesft-TEST.vcxproj /property:Configuration=Release
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
		- [x] 8x16 sprite rendering
	- [ ] Color emphasis & greyscale
- [x] Controller
	- [x] Keyboard default keys
	- [x] Keyboard custom keys
	- [x] Gamepad
- [ ] Audio
	- [x] APU
	- [x] Stream audio with OpenAL
	- [x] Changeable master volume
	- [ ] Filtering (AA and low-pass)
	- [ ] Fix OpenAL
		- [x] Sound latency
		- [ ] No sound if app closed unexpectedly...
	- [x] Generate sound signals
		- [x] Pulses
		- [x] Triangle
		- [x] Noise
		- [x] DMC
	- [x] Sound channels visualisation
	- [x] Sound spectrum visualisation
		- [ ] Zero padding
		- [ ] Window functions
- [ ] Mappers
	- [x] 000 (SMB, DK, Excitebike...)
	- [x] 001 (Zelda I & II, Metroid, Kid Icarus...)
	- [x] 002 (Castlevania, Duck Tales, Mega man...)
	- [x] 003 (Arkanoid, Gradius...)
	- [X] 004 (SMB2/3, Kirby's adventure, Mother)
	- [ ] Total: **5** / 256 iNES 1.0 mappers

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

### OpenGL
[Learn OpenGL](https://learnopengl.com/)

### Libraries
Check out [libraries](libraries) folder.

## License
MIT License. See [LICENSE](LICENSE) file.