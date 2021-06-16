# QtEA
Bachelorthesis Christoph Schirmel 2021

## Prerequisites

Generally, I recommend using QtCreator for building the application.

### Used Libraries / Tools

* Qt 5.15 (GPLv3) https://wiki.qt.io/Qt_5.15_Release
* OpenCV (Apache)
* C++11 / C++14 (recomm.) built with GCC/G++ (Linux) and MSVC2019 (Windows)

Not explicitly part of Qt-EApp, but recommended, when working with client certificates under Linux:
* certutil (GPLv2)

## Building Qt-EApp
1. Set up QtCreator (if not alredy done)
2. Open QtEA.pro in QtCreator
3. Eventually you need to adapt the opencv-includepath in QtEA.pro (sorry for that...)
4. Hit Compile
5. If you are under windows: Go to your build-directory and run `windeployqt.exe QtEA.exe` to get all shared objects in your build dir.
6. Check that opencv_world45<x>.so is also in your build directory where <x> is your installed version of opencv. You can find it in opencv/x64/vc15/ folder.

## Running Qt-EApp
Congrats, now you can run Qt-EApp. Per default it loads res/example-url.conf or so. If you want to use your config, simply run `QtEA.exe <config>` on Windows or `./QtEA <config>` on Linux.

