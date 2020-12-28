## Windows: Building from sources

This page is a step-by-step guide for building and installing in Windows. Please send a PR or send your patches and instructions for improving and fixing this building guide. Thank you.

Before starting, you should know that LenMus source tree does not include makefiles. All makefiles are generated with CMake build system. CMake can generate different kinds of native build files for many systems and IDEs (e.g. Unix Makefiles, Eclipse CDT 4.0 project files, Visual Studio project files, and many other).

#### Step 1: Install prerequisites

LenMus needs the following additional libraries:

- wxWidgets 3.0 or higher ([http://www.wxwidgets.org](http://www.wxwidgets.org))
- FreeType 2.3.5-1 or higher ([http://www.freetype.org/](http://www.freetype.org/))
- zlib ([http://zlib.net/](http://zlib.net/) and [http://gnuwin32.sourceforge.net/packages/zlib.htm](http://gnuwin32.sourceforge.net/packages/zlib.htm))
- libpng ( [http://www.libpng.org/pub/png/libpng.html](http://www.libpng.org/pub/png/libpng.html))
- portmidi ( [http://portmedia.sourceforge.net/portmidi/](http://portmedia.sourceforge.net/portmidi/))
- SQLite3 ( [https://www.sqlite.org/](https://www.sqlite.org/))
- FluidSynth ( [http://www.fluidsynth.org//](http://www.fluidsynth.org/))

In addition, you need:

- CMake version 3.1 or higher ([http://www.cmake.org](http://www.cmake.org)) for generating a Visual Studio project file or a Makefile.
- A Git client for downloading the sources ([https://git-scm.com/](https://git-scm.com/)).


For instructions to install them, please visit the official websites for these libraries. Please, take also into account that all libraries should be built with the same run-time library options `(/MD, /MDd, /MT or /MTd)`.


#### Step 2: Download LenMus sources

When the required libraries are installed, proceed to download the LenMus sources. Lets assume you have a folder named `projects` at `C:\Users\<your-user>\projects`. We are going to create there a sub-folder for all LenMus sources and download them from the master repository at GitHub.

For this open the File Explorer and go to the `projects` folder. Click on it with the mouse right button and select `Git Bash here`. This will open a command window suitable for using Git.

Now you can download either the latest code:

```
git clone -b master --single-branch --depth 1 https://github.com/lenmus/lenmus.git
```

or a specific Release (e.g. 5.6.0)

```
git clone -b `Release_5.6.0` --single-branch --depth 1 https://github.com/lenmus/lenmus.git
```

You can now close the command window. The downloaded source code will be in a folder named `lenmus` in `C:\Users\<your-user>\projects\lenmus`

#### Step 3: Generate a makefile and build

When the sources are downloaded run CMake and build. For this, two alternatives are suggested:

#### Option 1 (simpler): Build from command line using CMake and ninja

Open a Command Window appropriate for desired build type: x86 or x64 build.

- For x64 use "x64 Native Tools Command Prompt"
- For x86 use "Developer Command Prompt"

Create build directory:

```
cd C:\Users\<your-user>\projects\lenmus
rmdir zz-build /S /Q
mkdir zz-build && cd zz-build
```

Create the makefile, build and install:

```
cmake -G "Ninja" .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG"
ninja
ninja install
```

#### Option 2: Build from Visual Studio

- Open a the File Explorer and go to the `projects` folder containing the `lenmus` folder with the sources C:\Users\<your-user>\projects\`.
- Click with mouse right button on the `lenmus` folder and choose `Open in Visual Studio`.
- Visual Studio will open, will automatically execute CMake. When finished, press F7 (build all) and run the program with Ctrl+F5 (Start without debugging) or with F5 (Start).

Your comments and fixes to these instructions are welcome. Thank you.
