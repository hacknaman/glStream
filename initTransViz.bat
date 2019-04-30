:: none - x64 vs2013, x86 - x86 vs2013, UE - x64 vs2017

set BUILD_PARA=%1

IF "%BUILD_PARA%"=="x86" SET BUILD_ARCH=x86

call envset.bat 
call GenerateSolution.bat
cd ..