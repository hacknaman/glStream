call envset.bat
cd Test_Scripts

if %1 == 1 call AppTest.bat city
if %1 == 2 call AppTest.bat atlantis

cd ..