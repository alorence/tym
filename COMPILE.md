# Tag Your Music: Compile instructions

## Prerequisites

### Mandatory requirements

Before trying to compile Tag Your Music, you must ensure your system has the folowing softwares:

 - Qt 5
 - CMake (at least 2.8.9)

#### Windows

The only Microsoft Windows (c) compiler supported is vc11 (Visual Studio 2012). Others compilers (vc10, mingw-gcc 4.7)
should also work, but this is not guaranted in the future. Tag Your Music use some c++11 features, and the most
recent version of Microsoft Windows compiler is strongly advised to ensure compiling without errors.

To build a Microsoft Windows setup, you need to install Inno Setup (http://www.jrsoftware.org/isinfo.php).

#### MacOS

Tag Your Music properly compile on Mac OS 10.7 and 10.8 with the default compiler (clang). Because of the use of
c++11 features in the software, it is impossible to build an OS 10.6 compliant binary for now. Any solution is
welcome (I don't want to remove c++11 code ;)).

#### GNU/Linux

The default gcc version of modern GNU/Linux distributions should be ok. C++11 specific flags are added by CMake.

### Optional dependencies

If you want to generate documentation, you need to have Doxygen and Graphviz installed.

## Compiling commands

First, don't forget to update the external library submodule (from the project directory:

```bash
git submodule init
git submodule update
```

Then, you can generate the project build files with CMake. Here is an example for full command line build. Adapt
it with your needs and depending on the IDE you use. It is recommended to generate these files in a specific folder,
instead of directly in the project tree.

```bash
mkdir build
cd build
cmake ..
make
```

If you have any problem, please post a new ticket on [issue tracker](https://github.com/alorence/tym/issues "Tag Your Music issues")
and try to give as much information as possible:

 - Your system type and version (ex: Mac OS 10.7, Windows 8, Ubuntu 12.10)
 - The compiler you use, if you know it
 - The type and/or the text of the message you get
