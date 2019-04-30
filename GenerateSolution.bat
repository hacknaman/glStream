:: cmake should be added to the path before 
:: executing this command
if not "%BUILD_ARCH%"=="x86" goto generate64BitSolution
set FolderName=x86
if not exist %FolderName% mkdir %FolderName%
cd %FolderName%
If exist CMakeCache.txt del CMakeCache.txt
cmake -G "Visual Studio 12 2013" -A Win32 ../build ..
goto end

:generate64BitSolution
set FolderName=x64
if not exist %FolderName% mkdir %FolderName%
cd %FolderName%
If exist CMakeCache.txt del CMakeCache.txt

IF "%BUILD_PARA%"=="UE" ( 
	cmake -G "Visual Studio 15 2017 Win64" ../build ..
) 

IF NOT "%BUILD_PARA%"=="UE" ( 
	cmake -G "Visual Studio 12 2013 Win64" ../build ..
)

:end
