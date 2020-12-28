## macOS: Building from sources

This page is a step-by-step guide for building and installing in macOS. Please send a PR or send your patches and instructions for improving and fixing this building guide. Thank you.

> **Important** These instructions are yet experimental. They have been tested on a Mac running Catalina (MacOS 10.15) and Xcode version 11.5.  They will probably work on some earlier versions (an earlier version has been successfully built under MacOS 10.14 -- Mojave).

Before starting, you should know that LenMus source tree does not include makefiles. All makefiles are generated with CMake build system. CMake can generate different kinds of native build files for many systems and IDEs (e.g. Xcode, Unix Makefiles, Eclipse CDT 4.0 project files, Visual Studio project files, and many other).


### Step 1: Install prerequisites

For building LenMus you first will need the Xcode tools and the Homebrew package manager:


#### Xcode

Xcode is a free download from the App store, and contains the basic software development tools (C++ compiler, make, etc.) needed.  It includes both command-line tools and a very sophisticated IDE.  This needs to be installed first.


#### Homebrew

Install the Homebrew package manager.  This will allow easy installation of various libraries needed by LenMus.  See [https://brew.sh/](https://brew.sh/), or just open a terminal window and paste in:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

It should be possible (and is recommended) to install Homebrew as a regular user with administrative privileges (NOT using sudo). You can find discussion of this on the web if you run into problems.


#### wxWidgets libraries

Open a terminal window and type:
```
brew install wxwidgets
```


#### Other required libraries and utilities

LenMus requires the cmake utility, as well as some additional libraries. In a terminal window, use the Homebrew commands:
```
brew install cmake
brew install portmidi
brew install fluid-synth
brew install fontconfig
```

LenMus also requires sqlite3, libpng, zlib, and FreeType2. These should already be available on your Mac, but in case of problems you may wish to look for Homebrew alternatives.


#### FluidR3_GM.sf2 sound font

The sound font `FluidR3_GM.sf2` is required by the internal synthesizer and can be downloaded from 

[https://github.com/lenmus/lenmus/releases/download/Release_5.5.0/FluidR3_GM.sf2](https://github.com/lenmus/lenmus/releases/download/Release_5.5.0/FluidR3_GM.sf2)

It should be placed in the `res/sounds` folder of the LenMus source tree.



#### Step 2: Download LenMus sources

When the required tools and libraries are installed, proceed to download the LenMus sources. Let's assume you have a folder named 'projects'. We are going to create there a sub-folder for all LenMus sources and download them from master repository at GitHub, either the latest code:

```
cd projects
git clone -b master --single-branch --depth 1 https://github.com/lenmus/lenmus.git
cd lenmus
```

or a specific Release (e.g. 5.4.2)

```
cd projects
git clone -b '5.4.2' --single-branch --depth 1 https://github.com/lenmus/lenmus.git
cd lenmus
```


#### Step 3: Generate a makefile and build

When the sources are downloaded run CMake and build. For this, two alternatives are suggested:


#### Option 1 (simpler): Build from command line using CMake

Assuming you're in the top-level lenmus source folder, issue the commands to generate a `Makefile`:
```
 mkdir z_mybuild
 cd z_mybuild
 cmake ../
```

If no errors from the "cmake" command, issue:
```
make
```

Probably there will be some warnings from the make command, but hopefully no errors.



#### Option 2: Build using Xcode IDE development

Assuming you're in the top-level lenmus source folder, issue the commands to generate a `Makefile`:
```
mkdir z_mybuild
cd z_mybuild
cmake -G Xcode ../
```

This will generate an Xcode project file in the `z_mybuild` folder allowing one to continue using the Xcode IDE.  This would be preferred way for development and debugging, or just exploring the source code.  If you do this, after opening the project I suggest opening File--Project Settings, and changing the Build system to "New Build System".  This will enable debugging.  There will be some obvious Xcode targets corresponding to "make" and "make package".  The manual intervention in step 4 is still needed, and you may find the lenmus.app folder buried one level deeper in the bin folder.  Make the obvious changes.


#### Step 4: Preparing the app bundle

(This part will be automated as we get more knowledge and practice with the macOS system)  If there are no errors from the "make" command, inside your `z_mybuild` folder there should be a folder `bin` containing a folder hierarchy 
```
     bin --  lenmus.app -- Contents
```

Inside the folder `Contents` will be a file `Info.plist` and a folder `MacOS` containing the binary executable `lenmus`.  We need to add some support files.  So, starting from the `z_mybuild` folder, issue the commands:
```
 cd bin/lenmus.app/Contents
 mkdir -p Resources
 cp -r  ../../../../locale ./Resources/
 cp -r  ../../../../res ./Resources/
 cp -r  ../../../../templates ./Resources/
 cp -r  ../../../../xrc ./Resources/
```
For a complete distribution, also copy from the top lenmus folder the files AUTHORS, CHANGELOG.md, INSTALL, all the various license files (LICENSE, LICENSE_GNU_FDL_1.3.txt, LICENSE_GNU_GPL_1.3.txt, license_*.txt), NEWS, README.md, and THANKS.  The app will run without these but they should be included for completeness, especially if app is distributed.



#### Step 5: Testing the app bundle

Go back to the `z_mybuild/bin` folder containing `lenmus.app` (e.g., `cd ../..` from the previous step).  Test things out by issuing the command:
```
open ./lenmus.app
```

A functional version of lenmus should now open.  Test it out and exit it when finished testing.



#### Step 5: Final fix and install

If the test works, go back to the `z_mybuild` folder  (`cd ..` from the previous step) and issue the command 
```
make package
```

This should build an installation package with a name like `lenmus-x.x.x-Darwin.dmg` in the `z_mybuild` folder.  Double-click on it in the `Finder` to open it.  You can move the app to the `/Applications` folder, your desktop, or any other folder of your choice.

> **TROUBLESHOOTING:**  Command `make package` may fail.  As part of the package-building process, the shared libraries required by LenMus are copied into a `FrameWorks` folder in the app bundle and edited with the standard utility `install_name_tool` to change their install paths.  For this to work, the libraries must have been built with write permission for the owner (mode 0644).  Some Homebrew packages have their libraries' permissions incorrectly set to mode 0444.  To fix this:
```
cd /usr/local/Cellar
find . -type f -name \*.dylib | xargs chmod u+w
```

and try `make package` again from the `z_build` folder.




