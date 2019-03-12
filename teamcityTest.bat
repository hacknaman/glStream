call envset.bat
cd Test_Scripts

if %1 == 1 call CityTest.bat
if %1 == 2 call AtlantisTest.bat

cd ..