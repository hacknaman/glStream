:: cmake should be added to the path before 
:: executing this command

set FolderName=x64
if not exist %FolderName% mkdir %FolderName%
cd %FolderName%
If exist CMakeCache.txt del CMakeCache.txt
cmake -G "Visual Studio 12 2013 Win64" ../build ..

