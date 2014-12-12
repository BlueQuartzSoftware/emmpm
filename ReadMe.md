# Compiling EMMPM Library #

## Requirements (In order of Compilation)##

0. Compilers
1. [CMake](http://www.cmake.org)
2. [LibTiff](http://www.remotesensing.org)
3. [Boost 1.40 or Greater](http://www.boost.org) 
4. [CMP Source Codes](Master Branch from Git)
5. [MXADataModel Source Codes]( Master Branch from Git)

MXADataModel depends on Boost & CMP so those need to be compiled and installed first (just Boost. CMP just needs to be available at the same directory level as the EMMPM directory)

## Compiler Support ##

+ Visual Studio 2008, 2010, 2012
+ Xcode under OS X 10.6.8, 10.7.5, 10.8.5
+ Linux Distributions including CentOS 6.4, Ubuntu 11.x, OpenSUSE 11.x


## OS X/LINUX Shell commands (bash) ##


	[Build]$ export PATH=$PATH:/Users/Shared/Toolkits/cmake-2.8.12/CMake\ 2.8-12.app/Contents/bin
	[Build]$ export TIFF_INSTALL=/Users/Shared/Toolkits/tiff
	[Build]$ export BOOST_ROOT=/Users/Shared/Toolkits/boost-1.51.0
	[Build]$ git clone git://scm.bluequartz.net/CMP.git
	[Build]$ git clone git://scm.bluequartz.net/MXADataModel.git
	[Build]$ git clone git://scm.bluequartz.net/EMMPM.git
	[Build]$ cd EMMPM
	[Build]$ mkdir Build
	[Build]$ cd Build
	[Build]$ cmake -DCMAKE_BUILD_TYPE=Release ../
	[Build]$ make -j
	
## Compiling the EMMPM Workbench ##

Download [Qt 4.8.4](http://www.qt-project.org)
Either build Qt from scratch or use the prebuilt binaries from qt-project.org. Make sure the version of the libraries that you download match the version of Visual Studio that you are using. For OS X and Linux it does not really matter as long as you are running a recent version of the operating system and the compiler suite of tools.

Download an compile Qwt version 5.2.2 or greater. Their is a git repo for Qwt 5.2.2 from the bluequartz server.

	git clone git://scm.bluequartz.net/Qwt.git

Compile and install Qwt after Qt. Qwt provides the 2D plotting capabilities.

### More Environment Variables ###

	export QWT_INSTALL=/Users/Shared/Toolkits/Qwt-5.2.2

### Configure With CMake ###


	cmake -DCMAKE_BUILD_TYPE=Release -DQT_QMAKE_EXECUTABLE=/Users/Shared/Toolkits/Qt-4.8.4-Cocoa/bin/qmake ../

### Build EMMPMWorkbench ###

	make -j

### Create an Standalone Package ###

	make package


