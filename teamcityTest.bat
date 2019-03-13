::useage filename <testno> <useautoit:0/1>

echo "Running test no %1 ..."

call envset.bat
cd Test_Scripts

if %1 == 1 call AppTest.bat city %2
if %1 == 2 call AppTest.bat atlantis %2

cd ..
