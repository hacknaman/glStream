:: OSS ROOT PATH IS SET FROM TEAMCITY CLIENT

call FetchExternalDeps.bat
set OSSROOT=%cd%/ExternalLibrary
set "OSSROOT=%OSSROOT:\=/%"

:: SET OSS ROOT PATH
:: set OSG Directories
set OSG_DIR=%OSSROOT%/osg-3.4
set OSG_BIN=%OSG_DIR%/bin/%BUILD_ARCH%

:: set flexera libraries
set FLEXERA_DIR=%OSSROOT%/flexera/11.12
set FLEXERA_BIN=%FLEXERA_DIR%/bin/%BUILD_ARCH%

::set ServerContent Api libraries
set SERVER_CONTENT_API_DIR=%OSSROOT%/ServerAppContentApi
set SERVER_CONTENT_API_BIN = %SERVER_CONTENT_API_DIR%/bin/%BUILD_ARCH%
if not "%BUILD_ARCH%"=="x86" goto set64BitEnv 

:: set OSG Plugin Directories
set OSG_PLUGIN_DIR=%OSG_BIN%/osgPlugins-3.7.0

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%/glut
set GLUT_BIN=%GLUT_DIR%/bin
goto end

:set64BitEnv
set OSG_PLUGIN_DIR=%OSG_BIN%osgPlugins-3.4.2

::set GLUT Libraries
set GLUT_DIR=%OSSROOT%/glut
set GLUT_BIN=%GLUT_DIR%/bin\x64
goto end
:end

:: add external DLLs to the PATH
:: dynamic libs paths
set PATH=%OSG_BIN%;%OSG_PLUGIN_DIR%;%FLEXERA_BIN%;%GLUT_BIN%;%LIBJPEG_BIN%;%SERVER_CONTENT_API_BIN%;%PATH%

echo "================NEW BUILD==============="
del "x64\TransViz.sln"
del "x64\CMakeCache.txt"

mkdir build

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
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

:: This should be moved to package code
cmake --build . --config %buildType% --target INSTALL
cd ..

call copyDeps.bat

:: build ok
exit /b 0
