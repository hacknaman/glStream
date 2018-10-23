:: SET OSS ROOT PATH
set OSSROOT=E:\CHROMIUM64\ExternalLibrary

:: set OSG Directories
set OSG_DIR=%OSSROOT%\osg-3.4
set OSG_BIN=%OSG_DIR%\bin
set OSG_PLUGIN_DIR=%OSG_BIN%\osgPlugins-3.4.2

:: set flexera libraries
set FLEXERA_DIR=%OSSROOT%\flexera\11.12
set FLEXERA_BIN=%FLEXERA_DIR%\bin

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%\glut
set GLUT_BIN=%GLUT_DIR%\bin

:: set libjpeg libraries
set LIBJPEG_DIR=%OSSROOT%\libjpeg-turbo64
set LIBJPEG_BIN=%LIBJPEG_DIR%\bin

:: add external DLLs to the PATH
:: dynamic libs paths
set PATH=%OSG_BIN%;%OSG_PLUGIN_DIR%;%FLEXERA_BIN%;%GLUT_BIN%;%LIBJPEG_BIN%;%PATH%

:: set development mode
set Development=false

if "%1" EQU "debug" set DBG=1
if "%1" EQU "release" set RLS=1

set BUILDENV="Release"
if DEFINED DBG set BUILDENV="Debug"

if DEFINED RLS set BUILDENV="Release"

call GenerateSolution.bat