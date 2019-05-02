call FetchExternalDeps.bat
set BUILD_PARA=
call envset.bat 

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
