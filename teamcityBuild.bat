:: OSS ROOT PATH IS SET FROM TEAMCITY CLIENT

call FetchExternalDeps.bat
set OSSROOT=E:\agent\BuildAgent\work\TransViz\ExternalLibrary

:: set OSG Directories
set OSG_DIR=%OSSROOT%\osg-3.4
set OSG_BIN=%OSG_DIR%\bin
set OSG_PLUGIN_DIR=%OSG_BIN%\osgPlugins-3.4.2

:: set flexera libraries
set FLEXERA_DIR=%OSSROOT%\flexera\11.12
set FLEXERA_BIN=%FLEXERA_DIR%\bin

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%\glut
set GLUT_BIN=%GLUT_DIR%\bin\x64

:: set libjpeg libraries
set LIBJPEG_DIR=%OSSROOT%\libjpeg-turbo64
set LIBJPEG_BIN=%LIBJPEG_DIR%\bin

:: set Review API libraries
set REVIEW_DIR=%OSSROOT%\ReviewApi
set REVIEW_BIN=%REVIEW_DIR%\bin


:: set Catia API libraries
set CatiaCppApi_DIR=%OSSROOT%\CatiaCppApi
set CatiaCppApi_BIN=%CatiaCppApi_DIR%\bin

:: add external DLLs to the PATH
:: dynamic libs paths
set PATH=%OSG_BIN%;%OSG_PLUGIN_DIR%;%FLEXERA_BIN%;%GLUT_BIN%;%LIBJPEG_BIN%;%REVIEW_BIN%;%CatiaCppApi_BIN%;%PATH%

echo "================NEW BUILD==============="
del "x64\TransViz.sln"
del "x64\CMakeCache.txt"

mkdir build
:: set development mode
:: This is used for licensing
set Development=false

::if "%1" EQU "Debug" set DBG=1
::set BUILDENV=Release
::if DEFINED DBG set BUILDENV=Debug
::set buildType=Release
::if DEFINED DBG set buildType=Debug

set BUILDENV=%1
set buildType=%1

call GenerateSolution.bat
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
msbuild "TransViz.sln" /p:configuration=%buildType%

cmake --build . --config %buildType% --target INSTALL
cd ..

:: copy osg plugins
robocopy %OSSROOT%\osg-3.4\bin .\build\bin *.dll /MT:25
robocopy %OSSROOT%\osg-3.4\bin\osgPlugins-3.4.2 .\build\bin *.dll /MT:25

:: copy glut
robocopy %OSSROOT%\glut\bin\x64 .\build\bin *.dll /MT:25
