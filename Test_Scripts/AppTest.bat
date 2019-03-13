if %2==1 (
	start "FakeHuman" cmd /k FakeHuman.au3
)

SET testdir=%cd%
cd ..
cd build
SET builddir=%cd%
cd mothership
cd configs
start "testmothership" cmd /k python crRelease.conf %1 "%builddir%\apps" localhost scenegraph
cd ..
cd ..
echo %cd%
cd bin	
start "testcrappfaker" cmd /k crappfaker
SET errlevel=0

SET checkmodelpara = "" 
SET selftest = "" 

IF NOT [%3]==[]  (
	SET checkmodelpara=--checkmodel %testdir%\%3
) 

IF %2==0  (
	SET selftest=-t --appkilltime 10
)

Scenegraphapp %selftest% %checkmodelpara%

IF %ERRORLEVEL% NEQ 0 (
    SET errlevel=%ERRORLEVEL%
)

taskkill /FI "WindowTitle eq testmothership*" /T /F
taskkill /FI "WindowTitle eq testcrappfaker*" /T /F

IF %2==1 taskkill /FI "WindowTitle eq FakeHuman*" /T /F

echo %errlevel%
cd ..
exit /b %errlevel%
