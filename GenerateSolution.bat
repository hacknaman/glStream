# cmake should be added to the path before 
# executing this command

mkdir x64
cd x64
cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_INSTALL_PREFIX=.. ..