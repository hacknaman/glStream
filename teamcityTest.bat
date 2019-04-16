::useage filename <testno> <useFakeHuman:0/1> 

echo "Running test no %1 ..."

call envset.bat
cd Test_Scripts

if %1 == 1 call AppTest.bat simpleapp 0 NUL
if %1 == 2 call AppTest.bat city 1 NUL
if %1 == 3 call AppTest.bat atlantis 0 NUL atlantisTestModel.ive
if %1 == 4 call AppTest.bat cnext catia "E:\Program Files\Dassault Systemes\B20\win_b64\code\bin" engineTestModel.ive

cd ..
exit /b %errlevel%
